''' Dashboard for DataTaker sensor data 
Author: Alim Saidkhodjaev
'''
import dash
from dash.dependencies import Output, Input, State
from dash import dcc, html
import plotly.graph_objs as go
import pandas as pd
import time
import sys



# Initialize the Dash app
app = dash.Dash(__name__)

def read_csv_with_retries(file_path, row_interval, retries=5, delay=1):
    for i in range(retries):
        start_time = time.time()  # Record the start time
        try:
            # Get the total number of lines in the file
            with open(file_path, 'r') as file:
                total_lines = sum(1 for line in file)
            
            # Calculate the number of lines to skip
            skip_rows = max(0, total_lines - row_interval)

            # Read only the required lines
            df = pd.read_csv(file_path, skiprows=range(1, skip_rows + 1), delimiter=',',quotechar='"')  # Skip the header line for counting
            
            end_time = time.time()  # Record the end time
            elapsed_time = end_time - start_time
            print(f"Successfully read the file in {elapsed_time:.2f} seconds.")
            return df
        except Exception as e:
            end_time = time.time()  # Record the end time even if it fails
            elapsed_time = end_time - start_time
            print(f"Attempt {i+1} Error reading the file: {e} (took {elapsed_time:.2f} seconds)")
            time.sleep(delay)
    raise Exception("Failed to read the CSV file after several retries")


# Ensure the file path is provided
if len(sys.argv) < 2:
    print("Usage: python app.py <file_path>")
    sys.exit(1)

file_path = sys.argv[1]
print(f"Reading CSV file from: {file_path}")

# Read the CSV file once to get the initial structure
df = read_csv_with_retries(file_path, row_interval=500)  # Initial read with default row_interval
columns = [col for col in df.columns if (col != 'Timestamp' and col != 'TZ')]  # Exclude the timestamp column

print(f"Initial DataFrame columns: {df.columns}")
print(f"Columns to be plotted: {columns}")

# Initial layout for the app
app.layout = html.Div(
    [
        dcc.Interval(id='interval-component', interval=5000, n_intervals=0),
        dcc.Store(id='drawing-mode-store', data=False),  # Store for drawing mode state
        html.Div([
            html.Label("Time Interval To Plot (sec): "),
            dcc.Input(
                id='row-interval-input',
                type='number',
                value=500,  # Default value
                min=1,  # Minimum value
                style={'width': '5%', 'margin-right': '10px'}
            ),
            html.Label("Moving Average Window: "),
            dcc.Input(
                id='moving-average-input',
                type='number',
                value=10,  # Default moving average window
                min=1,  # Minimum value
                style={'width': '5%', 'margin-right': '10px'}
            ),
            html.Label("Update Interval (ms): "),
            dcc.Input(
                id='update-interval-input',
                type='number',
                value=5000,  # Default interval value
                min=100,  # Minimum value
                step=100,
                style={'width': '5%', 'margin-right': '10px'}
            ),
            html.Button('Set Interval', id='set-interval-button', n_clicks=0),
            html.Button('Toggle Drawing Mode', id='toggle-drawing-mode-button', n_clicks=0)  # Button to toggle drawing mode
        ]),
        html.Div([
            html.Label("Select Graphs to Display:"),
            dcc.Checklist(
                id='graph-toggle',
                options=[{'label': col, 'value': col} for col in columns],
                value=columns,  # Default to all graphs being visible
                inline=True
            )
        ]),
        html.Div(id='graphs-container')  # Container for graphs
    ]
)

@app.callback(
    Output('interval-component', 'interval'),
    [Input('set-interval-button', 'n_clicks')],
    [State('update-interval-input', 'value')]
)
def update_interval(n_clicks, new_interval):
    return new_interval

@app.callback(
    Output('drawing-mode-store', 'data'),
    [Input('toggle-drawing-mode-button', 'n_clicks')],
    [State('drawing-mode-store', 'data')]
)
def toggle_drawing_mode(n_clicks, drawing_mode):
    return not drawing_mode

@app.callback(
    Output('graphs-container', 'children'),
    [Input('interval-component', 'n_intervals'),
     Input('row-interval-input', 'value'),
     Input('moving-average-input', 'value'),
     Input('graph-toggle', 'value')]
)
def update_graph_layout(n, row_interval, moving_avg_window, selected_columns):
    try:
        # Read the CSV file
        df = read_csv_with_retries(file_path, row_interval)
        df['Timestamp'] = pd.to_datetime(df['Timestamp'])

        figures = []

        # Determine x-axis range based on Timestamp
        x_min = df['Timestamp'].min()
        x_max = df['Timestamp'].max()

        for col in selected_columns:
            # Determine y-axis range based on the selected column
            y_min = df[col].min()
            y_max = df[col].max()

            fig = go.Figure()

            # Plot the data for the column
            fig.add_trace(go.Scatter(x=df['Timestamp'], y=df[col], mode='lines', name=col))

            # Calculate and plot the moving average if the window size is greater than 0
            if moving_avg_window > 0:
                moving_avg = df[col].rolling(window=moving_avg_window).mean()
                fig.add_trace(go.Scatter(x=df['Timestamp'], y=moving_avg, mode='lines', name=f'{col} Moving Average', line=dict(width=4, dash='solid')))
            
            # Update the x-axis and y-axis ranges
            fig.update_layout(
                xaxis=dict(
                    title="Timestamp",
                    range=[x_min, x_max]  # Set the x-axis range to the min and max of the Timestamp
                ),
                yaxis=dict(
                    title=col,
                    range=[y_min, y_max]  # Set the y-axis range to the min and max of the data column
                ),
                title=f"{col}",
                clickmode='event+select'
            )

            figures.append(dcc.Graph(figure=fig, id={'type': 'graph', 'index': col}))

        return figures

    except Exception as e:
        print(f"Error updating the graph: {e}")
        return []

    
@app.callback(
    Output({'type': 'graph', 'index': dash.dependencies.ALL}, 'figure'),
    [Input({'type': 'graph', 'index': dash.dependencies.ALL}, 'clickData')],
    [State({'type': 'graph', 'index': dash.dependencies.ALL}, 'figure'),
     State('drawing-mode-store', 'data')]
)
def display_click_data(clickData, figures, drawing_mode):
    new_figures = []

    # Only proceed if drawing mode is active and at least one graph was clicked
    if drawing_mode and any(click is not None for click in clickData):
        # Find the first click data point (same across all plots)
        for click in clickData:
            if click is not None:
                x = click['points'][0]['x']  # The x-value where the line will be drawn
                break
        
        for fig in figures:
            if fig is not None:
                # Ensure the line covers the entire y-axis by using very large range
                y_min, y_max = -1e6, 1e6

                # Add the vertical line spanning the full y-axis range
                fig['data'].append(go.Scatter(
                    x=[x, x],
                    y=[y_min, y_max],
                    mode='lines',
                    name='Selected Point',
                    line=dict(color='Green', width=2)
                ))

            new_figures.append(fig)
    else:
        new_figures = figures

    return new_figures



if __name__ == '__main__':
    app.run_server(host="0.0.0.0")

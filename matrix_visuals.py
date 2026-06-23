import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

# Reads matrices separated by blank lines from file
def load_matrices(filename):
    matrices = []
    current = []
    with open(filename, 'r') as f:
        for line in f:
            stripped = line.strip()  # "A,B,A\n" --> "A,B,A"
            if stripped == "":  # Checks if line is empty - meaning new matrix next
                if current:
                    matrices.append(np.array(current))
                    current = []
            else:
                current.append(stripped.split(','))  # "A,B,A" --> ['A','B','A']

        if current:  # Last matrix if not followed by empty line
            matrices.append(np.array(current))
    return matrices

def generate_color_map(symbols):
    cmap = plt.get_cmap("tab20")  # Up to 20 distinct colors
    return {s: cmap(i)[:3] for i, s in enumerate(symbols)}

def matrix_to_rgb(matrix, color_map):
    rgb_array = np.zeros((len(matrix), len(matrix[0]), 3))
    for r, row in enumerate(matrix):
        for c, cell in enumerate(row):
            rgb_array[r, c] = color_map.get(cell, [1,1,1])  # Store color if found in array, otherwise white
    return rgb_array

def animate_matrices_manual(matrices, color_map):
    fig, ax = plt.subplots()
    grid_rgb = matrix_to_rgb(matrices[0], color_map)
    im = ax.imshow(grid_rgb)
    ax.axis('off')
    ax.set_title("Matrix Step 0")

    import matplotlib.patches as mpatches
    legend_patches = [mpatches.Patch(color=color_map[s], label=s) for s in color_map]
    ax.legend(handles=legend_patches, loc='upper right')

    paused = {'value': False}
    frame_idx = {'value': 0}

    def on_key(event):
        if event.key == ' ':  # toggle pause
            paused['value'] = not paused['value']
        elif event.key == 'n':  # next frame manually
            frame_idx['value'] = (frame_idx['value'] + 1) % len(matrices)
            im.set_array(matrix_to_rgb(matrices[frame_idx['value']], color_map))
            ax.set_title(f"Matrix Step {frame_idx['value']}")
            fig.canvas.draw_idle()

    fig.canvas.mpl_connect('key_press_event', on_key)

    # Automatic animation loop
    def update(frame):
        if not paused['value']:
            frame_idx['value'] = (frame_idx['value'] + 1) % len(matrices)
            im.set_array(matrix_to_rgb(matrices[frame_idx['value']], color_map))
            ax.set_title(f"Matrix Step {frame_idx['value']}")
        return [im]

    ani = animation.FuncAnimation(fig, update, interval=500)
    plt.show()

if __name__ == "__main__":
    filename = "matrices.csv"
    matrices = load_matrices(filename)
    print(f"Loaded {len(matrices)} matrices.")
    unique_symbols = sorted(set(cell for matrix in matrices for row in matrix for cell in row))
    color_map = generate_color_map(unique_symbols)
    animate_matrices_manual(matrices, color_map)
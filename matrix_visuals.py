import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as mpatches
import numpy as np


# Reads matrices separated by blank lines from file
def load_matrices(filename):
    matrices = []
    current = []

    with open(filename, 'r') as f:
        for line in f:
            stripped = line.strip()

            if stripped == "":
                if current:
                    matrices.append(np.array(current))
                    current = []
            else:
                current.append(stripped.split(','))

        if current:
            matrices.append(np.array(current))

    return matrices


# Reads species colors from the input file
def load_color_map(filename):
    color_map = {}
    reading_colors = False

    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()

            if line == "!START_COLORMAP":
                reading_colors = True
                continue

            if line == "!END_COLORMAP":
                break

            if reading_colors and line:
                # Example:
                # {Cargo 1} C1, GC1: (252,141,98)

                species_part = line.split('}')[1].split(':')[0].strip()
                color_part = line.split(':')[1].strip()

                # Remove parentheses
                color_part = color_part.strip('()')

                # Convert RGB 0-255 values to matplotlib 0-1 values
                rgb = [
                    int(value.strip()) / 255
                    for value in color_part.split(',')
                ]

                # Multiple species can share the same color
                species_list = species_part.split(',')

                for species in species_list:
                    species = species.strip()
                    color_map[species] = rgb

    return color_map


# Converts matrix species into RGB values for display
def matrix_to_rgb(matrix, color_map):
    rgb_array = np.zeros((len(matrix), len(matrix[0]), 3))

    for r, row in enumerate(matrix):
        for c, cell in enumerate(row):
            rgb_array[r, c] = color_map.get(cell, [1, 1, 1])

    return rgb_array


# Animates matrix simulation
def animate_matrices_manual(matrices, color_map):
    # Convert every matrix to RGB once before animation starts
    # This makes playback much faster
    rgb_frames = [
        matrix_to_rgb(matrix, color_map)
        for matrix in matrices
    ]

    fig, ax = plt.subplots()

    im = ax.imshow(rgb_frames[0])
    ax.axis('off')
    ax.set_title("Matrix Step 0")

    # Create legend
    legend_patches = [
        mpatches.Patch(
            color=color_map[species],
            label=species
        )
        for species in color_map
    ]

    # Move legend outside grid on right
    ax.legend(
        handles=legend_patches,
        loc='center left',
        bbox_to_anchor=(1.02, 0.5)
    )

    plt.tight_layout()

    paused = {'value': False}
    frame_idx = {'value': 0}

    # Shows current frame
    def show_frame():
        im.set_array(
            rgb_frames[frame_idx['value']]
        )

        ax.set_title(
            f"Matrix Step {frame_idx['value']}"
        )

        fig.canvas.draw_idle()

    # Keyboard controls
    def on_key(event):
        if event.key == ' ':
            # Pause / resume
            paused['value'] = not paused['value']

        elif event.key == 'right':
            # Move forward one frame
            frame_idx['value'] = (
                                         frame_idx['value'] + 1
                                 ) % len(rgb_frames)

            show_frame()

        elif event.key == 'left':
            # Move backward one frame
            frame_idx['value'] = (
                                         frame_idx['value'] - 1
                                 ) % len(rgb_frames)

            show_frame()

    fig.canvas.mpl_connect(
        'key_press_event',
        on_key
    )

    # Automatic animation
    def update(frame):
        if not paused['value']:
            frame_idx['value'] = (
                                         frame_idx['value'] + 1
                                 ) % len(rgb_frames)

            im.set_array(
                rgb_frames[frame_idx['value']]
            )

            ax.set_title(
                f"Matrix Step {frame_idx['value']}"
            )

        return [im]

    ani = animation.FuncAnimation(
        fig,
        update,
        interval=100,
        cache_frame_data=False
    )

    plt.show()

    return ani


if __name__ == "__main__":
    matrix_filename = "matrices.csv"
    input_filename = "input.csv"

    matrices = load_matrices(matrix_filename)

    print(f"Loaded {len(matrices)} matrices.")

    color_map = load_color_map(input_filename)

    ani = animate_matrices_manual(
        matrices,
        color_map
    )
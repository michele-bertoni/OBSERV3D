import matplotlib.pyplot as plt
from matplotlib.ticker import LinearLocator
from matplotlib import animation
from mpl_toolkits.mplot3d import Axes3D

import numpy as np
import json

def parse_heightmap_csv(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    ret = {'text': lines[0],
           'x': [],
           'y': [],
           'z': []}

    headers = lines[1].split(',')
    values = lines[2].split(',')
    for i in range(len(headers)):
        ret[headers[i]] = float(values[i])

    for l in lines[3:]:
        z = []
        for v in l.split(', '):
            z.append(float(v))
        ret['z'].append(z)

    x = ret['xmin']
    while x <= ret['xmax']:
        ret['x'].append(x)
        x += ret['xspacing']

    y = ret['ymin']
    while y <= ret['ymax']:
        ret['y'].append(y)
        y += ret['yspacing']

    return ret


def parse_heightmap_json(filename):
    json_dict = {'mode':'png'}
    try:
        with open(filename, 'r') as fp:
            json_dict = json.load(fp)
    except Exception as exc:
        print(exc)

    return json_dict


downloads_path = '/home/pi/Printy-McPrintface/Raspberry/.downloads/'
d = parse_heightmap_csv(downloads_path+"heightmap.csv")
p = parse_heightmap_json(downloads_path + "heightmap.json")

fig = plt.figure()
ax = fig.gca(projection='3d')

azim_elev = {}


def data_interpolation(x:list, y:list, z:list, num_iter=1):
    if num_iter <= 0:
        return x, y, z

    new_x, new_y, new_z = [], [], []
    for i in range(len(y)-1):
        new_y.append(y[i])
        new_y.append((y[i]+y[i+1])/2)
        z_i = []
        z_i1 = []
        for j in range(len(x)-1):
            z_i.append(z[i][j])
            z_i.append((z[i][j]+z[i][j+1])/2)
            z_i1.append((z[i][j]+z[i+1][j])/2)
            z_i1.append((z[i][j]+z[i+1][j]+z[i][j+1]+z[i+1][j+1])/4)
        z_i.append(z[i][-1])
        z_i1.append((z[i][-1]+z[i+1][-1])/2)
        new_z.append(z_i)
        new_z.append(z_i1)

    z_last = []
    for j in range(len(x) - 1):
        new_x.append(x[j])
        new_x.append((x[j]+x[j+1])/2)
        z_last.append(z[-1][j])
        z_last.append((z[-1][j]+z[-1][j+1])/2)
    z_last.append(z[-1][-1])
    new_z.append(z_last)

    new_x.append(x[-1])
    new_y.append(y[-1])

    return data_interpolation(new_x, new_y, new_z, num_iter-1)


def array_to_trisurf_coords(x:list, y:list, z:list):
    new_x, new_y, new_z = [], [], []

    for i in range(len(y)):
        for j in range(len(x)):
            new_x.append(x[j])
            new_y.append(y[i])
            new_z.append(z[i][j])

    return new_x, new_y, new_z


def heightmap_to_plot():
    x, y, z = d.get('x', []), d.get('y', []), d.get('z', [[]])
    x_bounds, y_bounds, z_bounds = p.get('x_bounds', [0, 250]), p.get('y_bounds', [0, 210]), p.get('z_bounds', [-0.25, 0.25])
    is_trisurf, num_interp = p.get('is_trisurf', True), p.get('num_interp', 2)

    x_p, y_p, z_p = array_to_trisurf_coords(x, y, z)
    x, y, z = data_interpolation(x, y, z, num_iter=num_interp)

    if is_trisurf:
        x, y, z = array_to_trisurf_coords(x, y, z)

    X_p = np.array(x_p)
    Y_p = np.array(y_p)
    Z_p = np.array(z_p)

    X = np.array(x)
    Y = np.array(y)
    Z = np.array(z)

    if not is_trisurf:
        X, Y = np.meshgrid(X, Y)

    # Plot the grid
    X_g = np.arange(x_bounds[0], x_bounds[1]+1, (x_bounds[1]-x_bounds[0])/10)
    Y_g = np.arange(y_bounds[0], y_bounds[1]+1, (y_bounds[1]-y_bounds[0])/10)
    for i in X_g:
        ax.plot(np.array([i, i]), np.array([Y_g.min(), Y_g.max()]), np.zeros(2), color='black', linewidth=1, alpha=1.0)
    for i in Y_g:
        ax.plot(np.array([X_g.min(), X_g.max()]), np.array([i, i]), np.zeros(2), color='black', linewidth=1, alpha=1.0)

    # Plot the surface
    ax.scatter(X_p, Y_p, Z_p, c=Z_p, cmap="Greys", vmin=z_bounds[0], vmax=z_bounds[1], linewidths=1)
    if is_trisurf:
        surf = ax.plot_trisurf(X, Y, Z, cmap="jet", linewidth=0, antialiased=False, vmin=z_bounds[0], vmax=z_bounds[1], alpha=0.9, shade=True)
    else:
        surf = ax.plot_surface(X, Y, Z, cmap="jet", linewidth=0, antialiased=False, vmin=z_bounds[0], vmax=z_bounds[1], alpha=0.9, shade=True)

    # Customize the axis.
    ax.set_xlim(x_bounds[0], x_bounds[1])
    ax.set_ylim(y_bounds[0], y_bounds[1])
    ax.set_zlim(z_bounds[0], z_bounds[1])
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_ticks(np.arange(round(z_bounds[0], 1), z_bounds[1], 0.1))

    # Add a color bar which maps values to colors.
    fig.colorbar(surf, shrink=0.8, aspect=10)

    return fig,


def heightmap_to_png():
    azim, elev = p.get('azim', -105), p.get('elev', 45)
    heightmap_to_plot()

    ax.view_init(elev=elev, azim=azim)
    plt.savefig(downloads_path + 'heightmap.png')


def animate(i):
    fps, duration = p.get('fps', 30), p.get('duration', 6)
    num_frames = fps * duration

    azim = azim_elev['a'][i]
    if 0 <= i < num_frames/4:
        elev = azim_elev['e1'][i]
    elif num_frames/4 <= i < num_frames*3/4:
        elev = azim_elev['e2'][i]
    else:
        elev = azim_elev['e3'][i]

    print(elev, azim, flush=True)
    ax.view_init(elev=elev, azim=azim)
    return fig,


def heightmap_to_mp4():
    azim, azim_d, elev = p.get('azim', -105), max(min(180, p.get('azim_d', 45)), -180), max(min(90, p.get('elev', 45)), -30)
    fps, duration = p.get('fps', 30), p.get('duration', 6)
    num_frames = fps * duration

    frame = np.arange(0, num_frames)
    azim_elev['a'] = azim - azim_d*np.sin(2*np.pi/num_frames*frame)
    azim_elev['e1'] = elev - (90-elev)/2*np.sin(2*np.pi/num_frames*frame)
    azim_elev['e2'] = elev + (90-elev)*np.sin(2*np.pi/num_frames*(frame-num_frames/4))
    azim_elev['e3'] = elev + (90-elev)/2*np.sin(2*np.pi/num_frames*frame)

    # Animate
    anim = animation.FuncAnimation(fig, animate, init_func=heightmap_to_plot,
                                   frames=num_frames, interval=20, blit=True)
    # Save
    anim.save(downloads_path+'heightmap.mp4', fps=fps, extra_args=['-vcodec', 'libx264'])


if __name__ == '__main__':
    try:
        if p['mode'] == 'png':
            print("Exporting png", flush=True)
            heightmap_to_png()

        elif p['mode'] == 'mp4':
            print("Exporting mp4", flush=True)
            heightmap_to_mp4()
    except Exception as exc:
        print(exc, flush=True)

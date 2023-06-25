# Wolfenstein 3D Raycasting

This repository contains a simple implementation of a raycasting algorithm inspired by the classic Wolfenstein 3D. Raycasting is a technique used to render 3D graphics in a 2D environment, giving the illusion of depth and perspective.

## Algorithm Overview

The algorithm works by casting rays from the player's position and calculating the distance to the walls in the environment. It then renders the scene creating a 3D effect (distant walls are rendered shorter and closer walls are taller). 
The provided code also allows you to visualize the rendering of the rays in a 2D scenario, and its resulting 3D output.

## Getting Started

To run the raycasting algorithm, follow these steps:

### Prerequisites

Before compiling and running this code, make sure you have the `SDL` library installed, you can install it on Debian-based distributions by running the following command:

```
apt install libsdl2-dev
```

### Compilation

To compile the code, make sure you have `make` installed. In the source directory run the following command:

```
make
```

### Usage

Run the `ray` executable after compilation:

```
./ray
```

### Cleanup

To remove the generated file, you can simply use the following command:

```
make clean
```


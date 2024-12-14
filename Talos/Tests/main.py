import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import serial

def draw_cube():
    vertices = [
        [-1, -1, -1],
        [ 1, -1, -1],
        [ 1,  1, -1],
        [-1,  1, -1],
        [-1, -1,  1],
        [ 1, -1,  1],
        [ 1,  1,  1],
        [-1,  1,  1]
    ]

    edges = [
        (0, 1), (1, 2), (2, 3), (3, 0),
        (4, 5), (5, 6), (6, 7), (7, 4),
        (0, 4), (1, 5), (2, 6), (3, 7)
    ]

    faces = [
        (0, 1, 2, 3),
        (4, 5, 6, 7),
        (0, 1, 5, 4),
        (2, 3, 7, 6),
        (0, 3, 7, 4),
        (1, 2, 6, 5)
    ]

    colors = [
        (1, 0, 0),  # Red
        (0, 1, 0),  # Green
        (0, 0, 1),  # Blue
        (1, 1, 0),  # Yellow
        (1, 0, 1),  # Magenta
        (0, 1, 1)   # Cyan
    ]

    glBegin(GL_QUADS)
    for i, face in enumerate(faces):
        glColor3fv(colors[i])
        for vertex in face:
            glVertex3fv(vertices[vertex])
    glEnd()

    glColor3f(1, 1, 1)  # Set color to white for edges
    glBegin(GL_LINES)
    for edge in edges:
        for vertex in edge:
            glVertex3fv(vertices[vertex])
    glEnd()

def read_serial_rotation(serial_port):
    try:
        line = serial_port.readline().decode('utf-8').strip()
        x, y, z = map(float, line.split(','))
        return x / 10000.0, z / -10000.0, y / -10000.0  # Convert from mdps to dps
    except Exception as e:
        print(f"Error reading serial data: {e}")
        return 0.0, 0.0, 0.0

def main():
    serial_port = serial.Serial('COM6', 115200)  # Change 'COM3' to your serial port

    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)

    gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(0.0, 0.0, -5)

    rotation_x = 0
    rotation_y = 0
    rotation_z = 0

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                serial_port.close()
                pygame.quit()
                return
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_r:
                    rotation_x = 0
                    rotation_y = 0
                    rotation_z = 0

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )

        delta_x, delta_y, delta_z = read_serial_rotation(serial_port)
        rotation_x += delta_x
        rotation_y += delta_y
        rotation_z += delta_z

        glPushMatrix()
        glRotatef(rotation_x, 1, 0, 0)
        glRotatef(rotation_y, 0, 1, 0)
        glRotatef(rotation_z, 0, 0, 1)
        draw_cube()
        glPopMatrix()

        pygame.display.flip()
        pygame.time.wait(10)

if __name__ == "__main__":
    main()

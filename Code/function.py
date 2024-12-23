import math


def function_angle_in_color(x):
    return -2*math.pi*x+2*math.pi

def function_angle_in_color_invert(x):
    return -(1/(2*math.pi))*x+1

print(function_angle_in_color_invert(3*math.pi/2))
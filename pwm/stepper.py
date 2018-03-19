import RPi.GPIO as GPIO
from time import sleep

DIR = 23   # Direction GPIO Pin
STEP = 18  # Step GPIO Pin
CW = 1     # Clockwise Rotation
CCW = 0    # Counterclockwise Rotation
SPR = 1600   # Steps per Revolution (360 / 1.8)

GPIO.setmode(GPIO.BCM)
GPIO.setup(DIR, GPIO.OUT)
GPIO.setup(STEP, GPIO.OUT)

step_count = SPR
rotation_count = 2
# delay = .0208
delay = .00001

GPIO.output(DIR, CW)
for y in range(rotation_count):
	for x in range(step_count):
	    GPIO.output(STEP, GPIO.HIGH)
	    sleep(delay)
	    GPIO.output(STEP, GPIO.LOW)
	    sleep(delay)

sleep(.5)
GPIO.output(DIR, CCW)
for y in range(rotation_count):
	for x in range(step_count):
	    GPIO.output(STEP, GPIO.HIGH)
	    sleep(delay)
	    GPIO.output(STEP, GPIO.LOW)
	    sleep(delay)

GPIO.cleanup()
import serial
import vgamepad as vg
import time

# UPDATE PORT IF NEEDED
SERIAL_PORT = "/dev/ttyUSB0"
BAUD_RATE = 115200
NUM_INPUTS = 13


def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
        print(f"Connected to {SERIAL_PORT}")
    except Exception as e:
        print(f"Error: {e}")
        return

    gamepad = vg.VX360Gamepad()
    print("Bridge running...")

    try:
        while True:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if not line:
                continue

            try:
                data = [int(val) for val in line.split(",")]
                if len(data) != NUM_INPUTS:
                    continue

                # Unpack all 13 values
                (start, a, b, x, y, lb, rb, lx, ly, cy, cx, lt, rt) = data

                # Buttons
                if a:
                    gamepad.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_A)
                else:
                    gamepad.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_A)
                if b:
                    gamepad.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_B)
                else:
                    gamepad.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_B)
                if x:
                    gamepad.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_X)
                else:
                    gamepad.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_X)
                if y:
                    gamepad.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_Y)
                else:
                    gamepad.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_Y)
                if lb:
                    gamepad.press_button(
                        button=vg.XUSB_BUTTON.XUSB_GAMEPAD_LEFT_SHOULDER
                    )
                else:
                    gamepad.release_button(
                        button=vg.XUSB_BUTTON.XUSB_GAMEPAD_LEFT_SHOULDER
                    )
                if rb:
                    gamepad.press_button(
                        button=vg.XUSB_BUTTON.XUSB_GAMEPAD_RIGHT_SHOULDER
                    )
                else:
                    gamepad.release_button(
                        button=vg.XUSB_BUTTON.XUSB_GAMEPAD_RIGHT_SHOULDER
                    )
                if start:
                    gamepad.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_START)
                else:
                    gamepad.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_START)

                # Left Stick (Pedals)
                gamepad.left_joystick(x_value=lx * 256, y_value=ly * 256)

                # Right Stick (C-Stick) - NOW USING CX AND CY
                gamepad.right_joystick(x_value=cx * 256, y_value=cy * 256)

                # Triggers
                gamepad.left_trigger(value=lt)
                gamepad.right_trigger(value=rt)

                gamepad.update()

            except ValueError:
                pass

    except KeyboardInterrupt:
        ser.close()


if __name__ == "__main__":
    main()

import umachine as machine
import utime as time

running = True

def key1_irq_callback(pin):
    if pin.value() == 0:
        print('Key1 is pressed')
    else:
        print('Key1 is released')

def key2_irq_callback(pin):
    global running
    running = False

if __name__ == '__main__':

    led = machine.Pin("LED", mode=machine.Pin.OUT)
    key1 = machine.Pin("KEY1", mode=machine.Pin.IN_PULLUP)
    key1.irq(trigger=machine.Pin.IRQ_RISING|machine.Pin.IRQ_FALLING, handler=key1_irq_callback)
    key2 = machine.Pin("KEY2", mode=machine.Pin.IN_PULLUP)
    key2.irq(trigger=machine.Pin.IRQ_FALLING, handler=key2_irq_callback)

    while running:
        led.on()
        time.sleep(0.5)
        led.off()
        time.sleep(0.5)

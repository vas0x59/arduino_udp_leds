#include <Arduino.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_NeoPixel.h>
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFA, 0xED};
IPAddress ip(192, 168, 1, 177);

unsigned int localPort = 8888; // local port to listen on

// buffers for receiving and sending data
char packetBuffer[128]; // buffer to hold incoming packet,
// char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

#define SERVOMIN 800
#define SERVOMAX 2100
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);

#define LED_PIN 7
int pixelFormat = NEO_GRB + NEO_KHZ800;
Adafruit_NeoPixel *pixels;

int servos[16];
int servo_count = 0;

int leds_count = 0;
byte leds[32][3];

void setServoPulse(uint8_t n, int pulse)
{
    double pulselength;

    pulselength = 1000000; // 1,000,000 us per second
    pulselength /= 60;     // 60 Hz
    // SerialUSB.print(pulselength);
    // SerialUSB.println(" us per period");
    pulselength /= 4096; // 12 bits of resolution
    // SerialUSB.print(pulselength);
    // SerialUSB.println(" us per bit");
    //   pulse *= 1000000;  // convert to us
    pulse /= pulselength;
    // SerialUSB.println(pulse);
    pwm1.setPWM(n, 0, pulse);
}
void set_servo(int servo_n, int angle)
{
    setServoPulse(servo_n, map(angle, 0, 255, SERVOMIN, SERVOMAX));
}

bool first_led_msg = true;

void led_callback()
{
    if ((leds_count != 0) && first_led_msg)
    {
        SerialUSB.println("Init LED");
        pixels = new Adafruit_NeoPixel(leds_count, LED_PIN, pixelFormat);
        pixels->begin();
        pixels->clear();
        first_led_msg = false;
    }
    for (int i = 0; i < leds_count; i++)
    { // For each pixel...

        pixels->setPixelColor(i, pixels->Color(leds[i][0], leds[i][1], leds[i][2]));
    }
    pixels->show();
}
void servo_callback()
{
    for (int i = 0; i < servo_count; i++)
    {
        set_servo(i, servos[i]);
    }
}
void callback()
{
    led_callback();
    servo_callback();
}

void parse(byte *packetBuffer, int packetSize)
{
    servo_count = packetBuffer[0];
    // int read = 1;
    // SerialUSB.print("s_count:");
    // SerialUSB.print(servo_count);
    // SerialUSB.print(" ");
    for (int i = 0; i < servo_count; i++)
    {
        servos[i] = packetBuffer[i + 1];
        // read++;
    }
    int led_p = servo_count + 1;
    leds_count = packetBuffer[led_p];
    // SerialUSB.print("leds_count:");
    // SerialUSB.print(leds_count);
    // SerialUSB.print(" ");
    for (int i = 0; i < leds_count * 3; i++)
    {
        leds[i / 3][i % 3] = packetBuffer[i + led_p + 1];
    }
}
void setup()
{
    SerialUSB.begin(9600);
    delay(5000);
    // put your setup code here, to run once:
    SerialUSB.println("Trying to get an IP address using DHCP");
    if (Ethernet.begin(mac) == 0)
    {
        SerialUSB.println("Failed to configure Ethernet using DHCP");
        // initialize the ethernet device not using DHCP:
    }
    // print your local IP address:
    SerialUSB.print("My IP address: ");
    ip = Ethernet.localIP();
    for (byte thisByte = 0; thisByte < 4; thisByte++)
    {
        // print the value of each byte of the IP address:
        SerialUSB.print(ip[thisByte], DEC);
        SerialUSB.print(".");
    }
    Udp.begin(localPort);
    pwm1.begin();
    pwm1.setPWMFreq(60);
}

void loop()
{
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        // SerialUSB.print("Received packet of size ");
        // SerialUSB.println(packetSize);
        // SerialUSB.print("From ");
        // IPAddress remote = Udp.remoteIP();
        // for (int i = 0; i < 4; i++)
        // {
        //     SerialUSB.print(remote[i], DEC);
        //     if (i < 3)
        //     {
        //         SerialUSB.print(".");
        //     }
        // }
        // SerialUSB.print(", port ");
        // SerialUSB.println(Udp.remotePort());

        // считываем пакет в буфер packetBufffer
        Udp.read(packetBuffer, packetSize);
        // SerialUSB.println("Contents:");
        // SerialUSB.println(packetBuffer);
        parse((byte *)packetBuffer, packetSize);
        callback();
    }
    delay(2);
}
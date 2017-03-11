import processing.serial.*;
import java.awt.datatransfer.*;
import java.awt.Toolkit;
import processing.opengl.*;
import saito.objloader.*;
import g4p_controls.*;

float roll  = 0.0F;
float pitch = 0.0F;
float yaw   = 0.0F;
float temp  = 0.0F;
float alt   = 0.0F;

OBJModel model;

// Serial port state.
Serial       port;
final String serialPort = "/dev/ttyUSB1";
boolean      printSerial = true;

void setup()
{
  size(640, 480, OPENGL);
  frameRate(30);
  model = new OBJModel(this);
  model.load("data/bunny.obj");
  model.scale(20);

  setSerialPort(serialPort);
}
 
void draw()
{
  background(0,0,0);

  // Set a new co-ordinate space
  pushMatrix();

  // Simple 3 point lighting for dramatic effect.
  // Slightly red light in upper right, slightly blue light in upper left, and white light from behind.
  pointLight(255, 200, 200,  400, 400,  500);
  pointLight(200, 200, 255, -400, 400,  500);
  pointLight(255, 255, 255,    0,   0, -500);
  
  // Move bunny from 0,0 in upper left corner to roughly center of screen.
  translate(300, 380, 0);
  
  // Rotate shapes around the X/Y/Z axis (values in radians, 0..Pi*2)
  //rotateZ(radians(roll));
  //rotateX(radians(pitch)); // extrinsic rotation
  //rotateY(radians(yaw));
  float c1 = cos(radians(roll));
  float s1 = sin(radians(roll));
  float c2 = cos(radians(pitch)); // intrinsic rotation
  float s2 = sin(radians(pitch));
  float c3 = cos(radians(yaw));
  float s3 = sin(radians(yaw));
  applyMatrix( c2*c3, s1*s3+c1*c3*s2, c3*s1*s2-c1*s3, 0,
               -s2, c1*c2, c2*s1, 0,
               c2*s3, c1*s2*s3-c3*s1, c1*c3+s1*s2*s3, 0,
               0, 0, 0, 1);

  pushMatrix();
  noStroke();
  model.draw();
  popMatrix();
  popMatrix();
  //print("draw");
}

void serialEvent(Serial p) 
{
  String incoming = p.readString();
  if (printSerial) {
    println(incoming);
  }
  
  if ((incoming.length() > 8))
  {
    String[] list = split(incoming, " ");
    if ( (list.length > 0) && (list[0].equals("Orientation:")) ) 
    {
      roll  = float(list[3]); // Roll = Z
      pitch = float(list[2]); // Pitch = Y 
      yaw   = float(list[1]); // Yaw/Heading = X
    }
    if ( (list.length > 0) && (list[0].equals("Alt:")) ) 
    {
      alt  = float(list[1]);
    }
    if ( (list.length > 0) && (list[0].equals("Temp:")) ) 
    {
      temp  = float(list[1]);
    }
    if ( (list.length > 0) && (list[0].equals("Calibration:")) )
    {
      int sysCal   = int(list[1]);
      int gyroCal  = int(list[2]);
      int accelCal = int(list[3]);
      int magCal   = int(list[4]);
    }
  }
}

// Set serial port to desired value.
void setSerialPort(String portName) {
  // Close the port if it's currently open.
  if (port != null) {
    port.stop();
  }
  try {
    // Open port.
    port = new Serial(this, portName, 9600);
    port.bufferUntil('\n');
  }
  catch (RuntimeException ex) {
    // Swallow error if port can't be opened, keep port closed.
    port = null; 
  }
}
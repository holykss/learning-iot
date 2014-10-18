import processing.serial.*;
import java.util.Arrays;

Serial myPort;  // Create object from Serial class

float [] RwAcc = new float[3];         //projection of normalized gravitation force vector on x/y/z axis, as measured by accelerometer
float [] Gyro = new float[3];          //Gyro readings

int lastTime = 0;
int interval = 0;

int lf = 10; // 10 is '\n' in ASCII
byte[] inBuffer = new byte[300];

PFont font;
final int VIEW_SIZE_X = 600, VIEW_SIZE_Y = 600;

void setup() 
{
  size(VIEW_SIZE_X, VIEW_SIZE_Y, P3D);
  //myPort = new Serial(this, "COM4", 115200);  
  myPort = new Serial(this, "/dev/tty.usbmodem202141", 115200);  
  myPort.bufferUntil('\n');
  font = loadFont("CourierNew36.vlw");
}

void draw() {
  background(#000000);
  drawInfo();
  drawGraphBackground();
  drawGraph();
}



void serialEvent (Serial myPort) {
  readSensors();
}

boolean readSensors() {
  boolean ret = false;
  int currentTime;

  if(myPort.available() > 0) {
    if (myPort.readBytesUntil(lf, inBuffer) > 0) {

      String inputString = new String(inBuffer);
        String [] inputStringArr = split(inputString, ',');
        if(inputStringArr.length == 6 ) {
          // convert raw readings to G
          RwAcc[0] = float(inputStringArr[0])/ 16384.0;
          RwAcc[1] = float(inputStringArr[1])/ 16384.0;
          RwAcc[2] = float(inputStringArr[2])/ 16384.0;
          
          // convert raw readings to degrees/sec
          Gyro[0] = float(inputStringArr[3]) / 131;
          Gyro[1] = float(inputStringArr[4]) / 131;
          Gyro[2] = float(inputStringArr[5]) / 131;
          
  
          // normalize data
          normalize3DVec(RwAcc);
          
          // interval
          currentTime = millis();
          interval = currentTime - lastTime;
          lastTime = currentTime;
  
          ret = true;
        }
        
        for(int i=0; i<inBuffer.length; i++){ inBuffer[i] = 0; }
      }
  }
  return ret;
}


void normalize3DVec(float [] vector) {
  float R;
  R = sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
  vector[0] /= R;
  vector[1] /= R;  
  vector[2] /= R;  
}



void drawInfo() {
  // Draw Info
  beginShape();
  fill(0,0,0);
  noStroke();
  rect(0,0, width, height/ 2 );

  fill(255,255,255);
  
  textFont(font, 20);
  text("RwAcc (G):\n" + RwAcc[0] + "\n" + RwAcc[1] + "\n" + RwAcc[2] + "\ninterval: " + interval, 20, 50);
  text("Gyro (°/s):\n" + Gyro[0] + "\n" + Gyro[1] + "\n" + Gyro[2], 220, 50);
  
  endShape();
}


int xPos = 0;
int yPos = 0;
long prev_time = 0;
long draw_interval = 5;
int [] DataRwAccX = null;
int [] DataRwAccY = null;
int [] DataRwAccZ = null;

int [] DataGyroX = null;
int [] DataGyroY = null;
int [] DataGyroZ = null;

int data_size = 100;

// graph variable  
int graph_width;
int graph_height;
int graph_x;
int graph_y;

void drawGraphBackground() {
  graph_width = width - 20;
  graph_height = 300;
  graph_x = 10;
  graph_y = height - 10 - graph_height;

  beginShape();
  stroke(255,255,255);
  strokeWeight(2);
  noFill();
  rect(graph_x, graph_y, graph_width, graph_height);
  line(graph_x, graph_y + graph_height / 2, graph_width + graph_x,  graph_y + graph_height / 2 );
  endShape();
}

void drawGraph() {
  // restrict time interval to draw
  long cur_time = millis();
  long dt = cur_time - prev_time; 
  if( dt < draw_interval ) return;
  prev_time = cur_time;

  xPos++;
  xPos %= data_size;
  
  if( DataRwAccX == null ) {
    DataRwAccX = new int[data_size];
    DataRwAccY = new int[data_size];
    DataRwAccZ = new int[data_size];
    DataGyroX = new int[data_size];
    DataGyroY = new int[data_size];
    DataGyroZ = new int[data_size];
    
    for(int i=0; i<data_size; ++i){DataRwAccX[i]=graph_y + graph_height / 2; }
    for(int i=0; i<data_size; ++i){DataRwAccY[i]=graph_y + graph_height / 2; }
    for(int i=0; i<data_size; ++i){DataRwAccZ[i]=graph_y + graph_height / 2; }
    for(int i=0; i<data_size; ++i){DataGyroX[i]=graph_y + graph_height / 2; }
    for(int i=0; i<data_size; ++i){DataGyroY[i]=graph_y + graph_height / 2; }
    for(int i=0; i<data_size; ++i){DataGyroZ[i]=graph_y + graph_height / 2; }
  }
  
  DataRwAccX[xPos] = (int)(map(RwAcc[0], -2.0f, 2.0f, graph_y, graph_y+graph_height));
  DataRwAccY[xPos] = (int)(map(RwAcc[1], -2.0f, 2.0f, graph_y, graph_y+graph_height));
  DataRwAccZ[xPos] = (int)(map(RwAcc[2], -2.0f, 2.0f, graph_y, graph_y+graph_height));

//  DataGyroX[xPos] = (int)(map(Gyro[0], -250f, 250f, graph_y, graph_y+graph_height));
//  DataGyroY[xPos] = (int)(map(Gyro[1], -250f, 250f, graph_y, graph_y+graph_height));
//  DataGyroZ[xPos] = (int)(map(Gyro[2], -250f, 250f, graph_y, graph_y+graph_height));
  
  beginShape();

  strokeWeight(4);
  noFill();
  
  float factor = (float)graph_width/data_size;
  for(int i=1; i<data_size; ++i){
    stroke(255,0,0); // red
    line((i-1) * factor + graph_x, DataRwAccX[i-1], i * factor + graph_x, DataRwAccX[i]  );
    stroke(0,255,0); // green
    line((i-1) * factor + graph_x, DataRwAccY[i-1], i * factor + graph_x, DataRwAccY[i]  );
    stroke(0,0,255); // blue 
    line((i-1) * factor + graph_x, DataRwAccZ[i-1], i * factor + graph_x, DataRwAccZ[i]  );
    stroke(255,255,0); // yellow
    line((i-1) * factor + graph_x, DataGyroX[i-1], i * factor + graph_x, DataGyroX[i]  );
    stroke(0,255,255); // cyan
    line((i-1) * factor + graph_x, DataGyroY[i-1], i * factor + graph_x, DataGyroY[i]  );
    stroke(255,0,255); // magenta
    line((i-1) * factor + graph_x, DataGyroZ[i-1], i * factor + graph_x, DataGyroZ[i]  );
  }

  endShape();
}




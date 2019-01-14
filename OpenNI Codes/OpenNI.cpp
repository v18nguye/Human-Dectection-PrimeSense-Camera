
#include "stdafx.h"
#include<stdlib.h>
#include <stdio.h>
#include<conio.h>
#include <OpenNI.h>
using namespace openni;
#include <NiTE.h>
#include <GL/glut.h>
#include<iostream>
using namespace std;

#include<opencv2/videoio/videoio.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>
using namespace cv;


// Declaring the global variables
int window_w = 640;
int window_h = 480;
OniRGB888Pixel* gl_texture;
OniRGB888Pixel* texturePixel;
nite::UserTracker uTracker_rt, uTracker1_rt;
VideoStream selectedSensor_rt, selectedSensor21_rt;
VideoStream selectedSensor2_rt, selectedSensor22_rt;
//Device device;
int k, n;

char ReadLastCharOfLine()
{
	int newChar = 0;
	int lastChar;
	fflush(stdout);
	do
	{
		lastChar = newChar;
		newChar = getchar();
	} while ((newChar != '\n') && (newChar != EOF));
	return (char)lastChar;
}
// Checking the errors for OppenNI
bool HandleStatus(Status status)
{
	if (status == STATUS_OK)
		return true;
	printf("ERROR: #%d, %s", status,
		OpenNI::getExtendedError());
	ReadLastCharOfLine();
	return false;
}

// Checking the errors for NiTe
bool HandleStatus(nite::Status status)
{
	if (status == nite::STATUS_OK)
		return true;
	printf("ERROR: #%d, %s", status,
		OpenNI::getExtendedError());
	ReadLastCharOfLine();
	return false;
}

// Select the colorsensor for the first device 
void SetActiveSensor_rt(SensorType sensorType, Device* device)
{
	Status status = STATUS_OK;
	printf("Checking if stream is supported ...\r\n");

	if (!device->hasSensor(sensorType))
	{
		printf("Stream not supported by this device.\r\n");
		return;
	}

	printf("Asking device to create a stream ...\r\n");
	status = selectedSensor_rt.create(*device, sensorType);
	if (!HandleStatus(status)) return;

	// Setting when use the realtime mode
	printf("Changing sensorcolorsenor video mode to 640x480@30fps.\r\n");
	VideoMode vmod;
	vmod.setFps(30);
	vmod.setPixelFormat(PIXEL_FORMAT_RGB888);
	vmod.setResolution(640, 480);
	status = selectedSensor_rt.setVideoMode(vmod);
	if (!HandleStatus(status)) return;
	printf("Done.\r\n");

	printf("Starting stream ...\r\n");
	status = selectedSensor_rt.start();
	if (!HandleStatus(status)) return;
	printf("Done.\r\n");
}

//Select the depthsensor for the first device
void SetActiveSensor2_rt(SensorType sensorType, Device* device)
{
	Status status = STATUS_OK;
	printf("Checking if stream is supported ...\r\n");

	if (!device->hasSensor(sensorType))
	{
		printf("Stream not supported by this device.\r\n");
		return;
	}

	printf("Asking device to create a stream ...\r\n");
	status = selectedSensor2_rt.create(*device, sensorType);
	if (!HandleStatus(status)) return;

	// Setting when use the real-time mode
	printf("Changing sensordepthsensor video mode to 640x480@30fps.\r\n");
	VideoMode vmod;
	vmod.setFps(30);
	vmod.setResolution(640, 480);
	vmod.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);
	status = selectedSensor2_rt.setVideoMode(vmod);
	if (!HandleStatus(status)) return;
	printf("Completed.\r\n");

	printf("Starting stream ...\r\n");
	status = selectedSensor2_rt.start();
	if (!HandleStatus(status)) return;
	printf("Done.\r\n");
}

//Select the colorsensor for the second device
void SetActiveSensor21_rt(SensorType sensorType, Device* device2)
{
	Status status = STATUS_OK;
	printf("Checking if stream is supported ...\r\n");

	if (!device2->hasSensor(sensorType))
	{
		printf("Stream not supported by this device.\r\n");
		return;
	}

	printf("Asking device to create a stream ...\r\n");
	status = selectedSensor21_rt.create(*device2, sensorType);
	if (!HandleStatus(status)) return;

	// Setting when use the realtime mode
	printf("Changing sensorcolorsenor video mode to 640x480@30fps.\r\n");
	VideoMode vmod;
	vmod.setFps(30);
	vmod.setPixelFormat(PIXEL_FORMAT_RGB888);
	vmod.setResolution(640, 480);
	status = selectedSensor21_rt.setVideoMode(vmod);
	if (!HandleStatus(status)) return;
	printf("Done.\r\n");

	printf("Starting stream ...\r\n");
	status = selectedSensor21_rt.start();
	if (!HandleStatus(status)) return;
	printf("Done.\r\n");
}

//Select the depthsensor for second device
void SetActiveSensor22_rt(SensorType sensorType, Device* device2)
{
	Status status = STATUS_OK;
	printf("Checking if stream is supported ...\r\n");

	if (!device2->hasSensor(sensorType))
	{
		printf("Stream not supported by this device.\r\n");
		return;
	}

	printf("Asking device to create a stream ...\r\n");
	status = selectedSensor22_rt.create(*device2, sensorType);
	if (!HandleStatus(status)) return;

	//Setting when using the real-time mode
	printf("Changing sensordepthsensor video mode to 640x480@30fps.\r\n");
	VideoMode vmod;
	vmod.setFps(30);
	vmod.setResolution(640, 480);
	vmod.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);
	status = selectedSensor22_rt.setVideoMode(vmod);
	if (!HandleStatus(status)) return;
	printf("Completed.\r\n");

	printf("Starting stream ...\r\n");
	status = selectedSensor22_rt.start();
	if (!HandleStatus(status)) return;
	printf("Done.\r\n");
}

// Detecting the first person in each frames.
int dtc1(nite::UserTrackerFrameRef  userfr)
{
	float a = 1000000;
	float m;

	const nite::Array<nite::UserData>& users =   // Get a list of active users
		userfr.getUsers();

	for (int i = 0; i < users.getSize(); ++i)
	{
		m = users[i].getCenterOfMass().z;
		if ((m <= a) && (m != 0))
		{
			a = users[i].getCenterOfMass().z;
			k = users[i].getId();
		}
	}
	return  k;
}

int dtc12(nite::UserTrackerFrameRef  userfr)
{
	float b = 1000000;
	float c;

	const nite::Array<nite::UserData>& users =   // Get a list of active users
		userfr.getUsers();

	for (int i = 0; i < users.getSize(); ++i)
	{
		c = users[i].getCenterOfMass().z;
		if ((c <= b) && (c != 0))
		{
			b = users[i].getCenterOfMass().z;
			n = users[i].getId();
		}
	}
	return  n;
}

// Ask for press ESC key to exist
void gl_KeyboardCallback(unsigned char key, int x, int y)
{
	if (key == 27) // ESC Key
	{
		uTracker_rt.destroy();
		uTracker1_rt.destroy();
		nite::NiTE::shutdown();
		exit(0);
	}
}

// Redisplay
void gl_IdleCallback()
{
	glutPostRedisplay();
}

// Display the colorframes and the depthframes on the texture
void gl_DisplayCallback_rt()
{
	//Checking the creation of userTracker is Valid. If the returne value is valid , then continue...
	if (uTracker_rt.isValid() && uTracker1_rt.isValid())
	{
		nite::Status niStatus = nite::STATUS_OK;

		// Read  directly the userframes by using OpenGL for the first device
		nite::UserTrackerFrameRef usersFrame;
		niStatus = uTracker_rt.readFrame(&usersFrame);

		// ....... for the second device
		nite::UserTrackerFrameRef usersFrame2;
		niStatus = uTracker1_rt.readFrame(&usersFrame2);

		// Checking userframe is valid to read, if valid ... continue.
		if (niStatus == nite::STATUS_OK && usersFrame.isValid() && usersFrame2.isValid())
		{
			// Initiating to read colorframe for the first device
			VideoFrameRef colorFrame;
			Status status = STATUS_OK;
			status = selectedSensor_rt.readFrame(&colorFrame);
			if (!HandleStatus(status)) return;

			// ... the second device
			VideoFrameRef colorFrame2;
			status = STATUS_OK;
			status = selectedSensor21_rt.readFrame(&colorFrame2);
			if (!HandleStatus(status)) return;

			// Clear the OpenGL buffers
			glClear(
				GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Setup the OpenGL viewpoint
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0, window_w, window_h, 0, -1.0, 1.0);

			// Initiating to read the depthframe for the first device
			VideoFrameRef depthFrame;
			status = STATUS_OK;
			status = selectedSensor2_rt.readFrame(&depthFrame);
			if (!HandleStatus(status)) return;

			// ... for the second device
			VideoFrameRef depthFrame2;
			status = STATUS_OK;
			status = selectedSensor22_rt.readFrame(&depthFrame2);
			if (!HandleStatus(status)) return;

			// Caculating and equalizing the histogram for the first device.
			int depthHistogram[65536];
			int numberOfPoints = 0;
			memset(depthHistogram, 0,
				sizeof(depthHistogram));
			for (int y = 0;
				y < depthFrame.getHeight(); ++y)
			{
				DepthPixel* depthCell = (DepthPixel*)(
					(char*)depthFrame.getData() +
					(y * depthFrame.getStrideInBytes())
					);
				for (int x = 0; x < depthFrame.getWidth();
					++x, ++depthCell)
				{
					if (*depthCell != 0)
					{
						depthHistogram[*depthCell]++;
						numberOfPoints++;
					}
				}
			}
			for (int nIndex = 1;
				nIndex < sizeof(depthHistogram) / sizeof(int);
				nIndex++)
			{
				depthHistogram[nIndex] +=
					depthHistogram[nIndex - 1];
			}

			//... for the second device
			int depthHistogram2[65536];
			int numberOfPoints2 = 0;
			memset(depthHistogram2, 0,
				sizeof(depthHistogram2));
			for (int y = 0;
				y < depthFrame2.getHeight(); ++y)
			{
				DepthPixel* depthCell2 = (DepthPixel*)(
					(char*)depthFrame2.getData() +
					(y * depthFrame2.getStrideInBytes())
					);
				for (int x = 0; x < depthFrame2.getWidth();
					++x, ++depthCell2)
				{
					if (*depthCell2 != 0)
					{
						depthHistogram2[*depthCell2]++;
						numberOfPoints2++;
					}
				}
			}
			for (int nIndex2 = 1;
				nIndex2 < sizeof(depthHistogram2) / sizeof(int);
				nIndex2++)
			{
				depthHistogram2[nIndex2] +=
					depthHistogram2[nIndex2 - 1];
			}

			// Caculating and selecting the resizefactor
			double resizeFactor = min(
				(window_w / ((double)depthFrame.getWidth())),
				(window_h / ((double)depthFrame.getHeight())));
			unsigned int texture_x = (unsigned int)(window_w -
				(resizeFactor * depthFrame.getWidth())) / 2;
			unsigned int texture_y = (unsigned int)(window_h -
				(resizeFactor * depthFrame.getHeight())) / 2;

			// Find the pixels which belong to user
			nite::UserMap usersMap = usersFrame.getUserMap(); // Create the userMap which contain all the information about user for the first device
			nite::UserMap usersMap2 = usersFrame2.getUserMap(); // ... for the second device

																// UPDATING TEXTURE
			for (unsigned int y = 0;
				y < (window_h - 2 * texture_y); ++y)
			{
				texturePixel = gl_texture +
					((y + texture_y) * window_w) + texture_x;

				for (unsigned int x = 0;
					x < (window_w - 2 * texture_x);
					++x, ++texturePixel)

				{
					// Updating the first stream 
					if (x < (window_w - 2 * texture_x) / 2)
					{
						DepthPixel* depthPixel =
							(DepthPixel*)(
							(char*)depthFrame.getData() +
								((int)(y / resizeFactor) *
									depthFrame.getStrideInBytes())
								) + (int)(2 * (x / resizeFactor));

						RGB888Pixel* colorPixel =
							(RGB888Pixel*)(
							(char*)colorFrame.getData() +
								((int)(y / resizeFactor) *
									colorFrame.getStrideInBytes())
								) + (int)(2 * (x / resizeFactor));

						nite::UserId* userPixel =
							(nite::UserId*)(
							(char*)usersMap.getPixels() +
								((int)(y / resizeFactor) *
									usersMap.getStride())
								) + (int)(2 * (x / resizeFactor));

						if (*depthPixel != 0) //*depthpixel is a distance in real space
						{
							if (*userPixel == dtc1(usersFrame))
							{

								float depthValue =
									(1 - ((float)depthHistogram[*depthPixel]
										/ numberOfPoints));

								texturePixel->b = ceil((double)
									(colorPixel->b* depthValue));
								texturePixel->g = ceil((double)
									(colorPixel->g* depthValue));
								texturePixel->r = ceil((double)
									(colorPixel->r* depthValue));
							}
							else
							{
								texturePixel->b = 0;
								texturePixel->g = 0;
								texturePixel->r = 0;
							}
						}
						else
						{
							texturePixel->b = 0;
							texturePixel->g = 0;
							texturePixel->r = 0;
						}
					}
					// Updating the second stream
					else
					{
						DepthPixel* depthPixel2 =
							(DepthPixel*)(
							(char*)depthFrame2.getData() +
								((int)(y / resizeFactor) *
									depthFrame2.getStrideInBytes())
								) + (int)(2 * ((x - (window_w - 2 * texture_x) / 2) / resizeFactor));

						RGB888Pixel* colorPixel2 =
							(RGB888Pixel*)(
							(char*)colorFrame2.getData() +
								((int)(y / resizeFactor) *
									colorFrame2.getStrideInBytes())
								) + (int)(2 * ((x - (window_w - 2 * texture_x) / 2) / resizeFactor));

						nite::UserId* userPixel2 =
							(nite::UserId*)(
							(char*)usersMap2.getPixels() +
								((int)(y / resizeFactor) *
									usersMap2.getStride())
								) + (int)(2 * ((x - (window_w - 2 * texture_x) / 2) / resizeFactor));

						if (*depthPixel2 != 0)
						{
							if (*userPixel2 == dtc12(usersFrame2))
							{
								float depthValue2 =
									(1 - ((float)depthHistogram[*depthPixel2]
										/ numberOfPoints));

								texturePixel->b = ceil((double)
									(colorPixel2->b * depthValue2));
								texturePixel->g = ceil((double)
									(colorPixel2->g * depthValue2));
								texturePixel->r = ceil((double)
									(colorPixel2->r * depthValue2));
							}
							else
							{
								texturePixel->b = 0;
								texturePixel->g = 0;
								texturePixel->r = 0;
							}
						}
						else
						{
							texturePixel->b = 0;
							texturePixel->g = 0;
							texturePixel->r = 0;
						}
					}
				}
			}

			// Create the OpenGL texture map
			glTexParameteri(GL_TEXTURE_2D,
				0x8191, GL_TRUE); // 0x8191 = GL_GENERATE_MIPMAP
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
				window_w, window_h, 0, GL_RGB,
				GL_UNSIGNED_BYTE, gl_texture);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.0f, (float)window_h, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f((float)window_w,
				(float)window_h, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f((float)window_w, 0.0f, 0.0f);
			glEnd();
			glutSwapBuffers();
		}
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	Status status = STATUS_OK;
	printf("Scanning machine for devices and loading "
		"modules/drivers ...\r\n");

	status = openni::OpenNI::initialize();
	if (!HandleStatus(status)) return 1;
	printf("Completed.\r\n");

	/*char* addressOfFile =
	"../../data/videoc2.oni";
	printf("Opening ONI file from %s as device ...\r\n",
	addressOfFile);

	char* addressOfFile2 =
	"../../data/videoc2.oni";
	printf("Opening ONI file from %s as device ...\r\n",
	addressOfFile2);*/

	openni::Device device_rt, device2_rt;

	status = device_rt.open(ANY_DEVICE);
	if (!HandleStatus(status)) return 1;
	printf("%s Opened the first Oni file, Completed.\r\n",
		device_rt.getDeviceInfo().getName());

	status = device2_rt.open(ANY_DEVICE);
	if (!HandleStatus(status)) return 1;
	printf("%s Opened the second Oni file, Completed.\r\n",
		device2_rt.getDeviceInfo().getName());

	SetActiveSensor_rt(SENSOR_COLOR, &device_rt);

	SetActiveSensor2_rt(SENSOR_DEPTH, &device_rt);

	SetActiveSensor21_rt(SENSOR_COLOR, &device2_rt);

	SetActiveSensor22_rt(SENSOR_DEPTH, &device2_rt);

	nite::Status niStatus = nite::STATUS_OK;
	printf("Initializing NiTE ...\r\n");
	niStatus = nite::NiTE::initialize();

	printf("Creating a user tracker object for first device ...\r\n");
	niStatus = uTracker_rt.create(&device_rt);
	if (!HandleStatus(niStatus)) return 1;
	printf("Done.\r\n");

	printf("Creating a user tracker object for sencond device ...\r\n");
	niStatus = uTracker1_rt.create(&device2_rt);
	if (!HandleStatus(niStatus)) return 1;
	printf("Done.\r\n");

	// Setting when use  the real-time mode
	status = STATUS_OK;
	printf("Enabling Frame Sync ...\r\n");
	status = device_rt.setDepthColorSyncEnabled(true);
	if (status != STATUS_OK)
	{
		printf("ERROR: #%d, %s", status, OpenNI::getExtendedError());    return 1;
	}
	printf("Enabling Depth to Image mapping\r\n");
	status = device_rt.setImageRegistrationMode(
		IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	if (!HandleStatus(status)) return 1;

	printf("Initializing OpenGL ...\r\n");
	gl_texture = (OniRGB888Pixel*)malloc(
		window_w * window_h * sizeof(OniRGB888Pixel));
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window_w, window_h);
	glutCreateWindow("Do an Tin Hoc ");
	glutKeyboardFunc(gl_KeyboardCallback);
	glutDisplayFunc(gl_DisplayCallback_rt);
	glutIdleFunc(gl_IdleCallback);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	printf("Starting OpenGL rendering process ...\r\n");
	glutMainLoop();
}

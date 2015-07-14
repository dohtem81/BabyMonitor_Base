/*
 *   This file is part of BabyMonitor.
 *
 *   BabyMonitor is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   any later version.
 *
 *   BabyMonitor is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with IONet.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   For any comment or suggestion please contact the author of that software
 *   at pedziwiatr.piotr@gmail.com
 */

#include <iostream>
#include <fstream>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

using namespace std;


static int xioctl(int fd, int request, void *arg)
{
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}


int main() {

	cout << "BabyMobitor Server application" << endl ;

	// camera file
	ifstream ifs;
	ifs.open ("/dev/video0", ifstream::in);

	// check if file is opened
	if (!ifs.is_open()){
		cout << "[ERROR]\tcould not open camera file" << endl ;
		return -1 ;
	} else {
		cout << "[INFO]\topened camera file successful" << endl ;
		ifs.close();
	}
	// camera exists and can be opened

	// now this part of the code relies on C style
	// because of ioctl function
	// in this section we check waht camera can do and
	// we set it up


	// check camera cababilities
	struct v4l2_capability caps = {0};
	int fd ;
	fd = open("/dev/video0", O_RDWR);
	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &caps))
	{
	    cout << "[ERROR]\tquerying capabilites" << endl ;
	    return -1;
	} else
		cout << "[INFO]\tgot capabilites" << endl ;

	// check for V4L2_CAP_READWRITE capability
	cout << "[INFO]\tV4L2_CAP_READWRITE " ;
	if ( !(caps.capabilities & V4L2_CAP_READWRITE) )
		cout << "not OK" << endl ;
	else
		cout << "OK" << endl ;

	cout << "[INFO]\tV4L2_CAP_STREAMING " ;
	if ( !(caps.capabilities & V4L2_CAP_STREAMING) )
		cout << "not OK" << endl ;
	else
		cout << "OK" << endl ;

	//

	// set frame format
	struct v4l2_format fmt ;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 1280;
	fmt.fmt.pix.height = 960;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
	{
		cout << "[ERROR]\tsetting pixel format failed" << endl ;
		return -1;
	} else
		cout << "[INFO]\tset pixel format" << endl ;

	// request buffer
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
    {
        cout << "[ERROR]\trequesting buffer failed" << endl ;
        return -1;
    } else
    	cout << "[INFO]\trequested buffer ok" << endl ;


	// query buffer
	struct v4l2_buffer buf = {0};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;
	if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
	{
		cout << "[ERROR]\tquering buffer failed" << endl ;
	    return -1;
	} else
		cout << "[INFO]\tgot buffer" << endl ;

	uint8_t *buffer;
	buffer = (uint8_t*)mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

    if(-1 == xioctl(fd, VIDIOC_QBUF, &buf))
    {
        cout << "[ERROR]\tQBUF failed" << endl ;
        return -1;
    }

	if(-1 == xioctl(fd, VIDIOC_STREAMON, &buf.type))
	{
	    cout << "[ERROR]\tstreaming failed" << endl ;
	    return -1;
	}

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    struct timeval tv = {0};
    tv.tv_sec = 2;
    int r = select(fd+1, &fds, NULL, NULL, &tv);
    if(-1 == r)
    {
        cout << "[ERROR]\twaiting for frame failed" << endl ;
        return -1;
    }

    if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
    {
        cout << "[ERROR]\tretriving frame failed" << endl ;
        return -1;
    }

    int outfd = open("./out.img", O_RDWR);
    write(outfd, buffer, buf.bytesused);
    close(outfd);


	return 0;
}



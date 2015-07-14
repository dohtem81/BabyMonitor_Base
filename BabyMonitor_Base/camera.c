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

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>


// wrapper function for ioctl()
// this function is copied from https://gist.github.com/Circuitsoft/1126411
// library based on same source https://gist.github.com/Circuitsoft/1126411
// parameters:
//		int fd						file descriptor to device
//		int request					request to the driver/device
//		void *arg					argument for ioctl function
// returns:
//		int							result from ioctl
///////////////////////////////////////////////////////////////////////////////
static int xioctl(int fd, int request, void *arg)
{
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}


// check camera capabilities
// this function populates v4l2_capability structure
// parameters:
//		int fd						file descriptor to device
//		int queriedCap				capability that is beeing checked
//		v4l2_capability caps		request to the driver/device
//		void *arg					argument for ioctl function
// returns:
//      1							capability exists
//		0							capability does not exist
//									or some other error
///////////////////////////////////////////////////////////////////////////////
static int queryCameraCap(int *fd, int queriedCap, struct v4l2_capability *caps) {

	if (-1 == xioctl(*fd, queriedCap, caps))
	{
	    return 0 ;
	} else
		return 1 ;
}


// set frame format
// this function sets frame format from the camera
// parameters:
//		int fd						file descriptor to device
//		int queriedCap				capability that is beeing checked
//		v4l2_capability caps		request to the driver/device
//		void *arg					argument for ioctl function
// returns:
//      1							success
//		0							failed
///////////////////////////////////////////////////////////////////////////////
static int setFrameFormat(int *fd, struct v4l2_format *fmt) {

	if (-1 == xioctl(*fd, VIDIOC_S_FMT, fmt))
		return 0 ;
	else
		return 1 ;

}


// set frame format
// this function sets frame format from the camera
// parameters:
//		int fd						file descriptor to device
//		int queriedCap				capability that is beeing checked
//		v4l2_capability caps		request to the driver/device
//		void *arg					argument for ioctl function
// returns:
//      1							success
//		0							failed
///////////////////////////////////////////////////////////////////////////////
static int videoBuffer(int *fd, int request, struct v4l2_buffer *buf) {

    if(-1 == xioctl(*fd, request, buf))
        return 0 ;
    else
    	return 1 ;
}



#include "ImgIdentify.h"
/*---------------------------------------------------------------------------*/
ImgIdentify::ImgIdentify()
{
}
/*---------------------------------------------------------------------------*/
ImgIdentify::~ImgIdentify()
{
}
/*---------------------------------------------------------------------------*/
double ImgIdentify::angle(Point pt1, Point pt2, Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
/*---------------------------------------------------------------------------*/
//finds the smaller square-box with reference to biggest_square, the relation is given by the multifying factor "scale_factor"
//in ovd kvd: outer rectangle in identify pattern is 3 times bigger than inner rectangle, hence scale factor to be used is 3
int ImgIdentify::find_relative_square(const vector<vector <Point> >& squares, vector<Point> biggest_square,vector<Point>& relative_square,int scale_factor,int tolerence) 
{
	if (!squares.size()) 
		return -1;
	int max_width = 0;
	int max_height = 0;
	int max_square_idx = 0;
	const int n_points = 4;

	Rect relative_rect = boundingRect(Mat(biggest_square));
	cout<<"rel.width="<<relative_rect.width<<" rel.height="<<relative_rect.height<<endl;
	relative_rect.width/=scale_factor;
	relative_rect.height/=scale_factor;

	for (size_t i = 0; i < squares.size(); i++) 
	{
		Rect rectangle = boundingRect(Mat(squares[i]));
		if(rectangle.width<MIN_WIDTH_THRESHOLD)//ignore too small squares
			continue;

//cout<<"i="<<i<<" : rect.width="<<rectangle.width<<" : rect.height="<<rectangle.height<<endl;
		//check if small rect fits within the tolerence window
		if( (rectangle.width >= relative_rect.width-tolerence) && (rectangle.width <= relative_rect.width+tolerence) )
		{
			float tmp=tolerence*0.5;//fullHD H to W aspect ratio is 9/16=0.576(hence reduce the height-tolerance to half)
			int tolH=(int)tmp;
			if( (rectangle.height >= relative_rect.height-tolH) && (rectangle.height <= relative_rect.height+tolH) )
			{
				relative_square = squares[i];
				return 0;
			}
		}
	}
	return -1;
}
int ImgIdentify::find_relative_square_new(const vector<vector <Point> >& squares, vector<Point> biggest_square,vector<Point>& relative_square,float scale_W,float scale_H,float tolerence) 
{
	if (!squares.size()) 
		return -1;
	int max_width = 0;
	int max_height = 0;
	int max_square_idx = 0;
	const int n_points = 4;

	float tmp_width,tmp_height;

	Rect relative_rect = boundingRect(Mat(biggest_square));
//cout<<"ref.width="<<relative_rect.width<<" ref.height="<<relative_rect.height<<endl;
LOG_DEBUG_MSG_2_ARG(logflag,"BRBOX:smarteye","ImgIdentify::find_relative_square_new:refW=%d refH=%d",relative_rect.width,relative_rect.height);

	for (size_t i = 0; i < squares.size(); i++) 
	{
		Rect rectangle = boundingRect(Mat(squares[i]));
		if(rectangle.width<MIN_WIDTH_THRESHOLD)//ignore too small squares
			continue;

		tmp_width =(float)relative_rect.width/rectangle.width;
		tmp_height=(float)relative_rect.height/rectangle.height;
LOG_DEBUG_MSG_2_ARG(logflag,"BRBOX:smarteye","ImgIdentify::find_relative_square_new:valueW=%d valueH=%d",rectangle.width,rectangle.height);
LOG_DEBUG_MSG_2_ARG(logflag,"BRBOX:smarteye","ImgIdentify::find_relative_square_new:ratioW=%f ratioH=%f",tmp_width,tmp_height);
//cout<<"tmpW="<<tmp_width<<" tmpH="<<tmp_height<<" W="<<rectangle.width<<" H="<<rectangle.height<<endl;
		if( (tmp_width >= (float)(scale_W-tolerence) ) && (tmp_width <= (float)(scale_W+tolerence)) )
		{
			if( (tmp_height >= (float)(scale_H-tolerence) ) && (tmp_height <= (float)(scale_H+tolerence)) )
			{
				relative_square = squares[i];
				return 0;
			}
		}
	}
	return -1;
}
/*---------------------------------------------------------------------------*/
//finds the larges square from squares-list
int ImgIdentify::find_largest_square(const vector<vector <Point> >& squares, vector<Point>& biggest_square,float ratioLow,float ratioHigh) 
{
	if (!squares.size()) 
		return -1;

	int max_width = 0;
	int max_height = 0;
	int max_square_idx = 0;
	const int n_points = 4;

	float WHratio;

	for (size_t i = 0; i < squares.size(); i++) 
	{
		Rect rectangle = boundingRect(Mat(squares[i]));
		WHratio=(float)((float)rectangle.width/(float)rectangle.height);
		if ((rectangle.width >= max_width) && (rectangle.height >= max_height) && (WHratio>=ratioLow) && (WHratio<=ratioHigh)) 
		{
			max_width = rectangle.width;
			max_height = rectangle.height;
			max_square_idx = i;
		}
	}
	biggest_square = squares[max_square_idx];
	return 0;
}
/*---------------------------------------------------------------------------*/
int ImgIdentify::apply_threshold(Mat& src_gray,int trsh,RNG &rng,Mat& drawing, vector<vector<Point> > &contours)
{
	Mat threshold_output;
	vector<Vec4i> hierarchy;

	/// Detect edges using Threshold
	threshold( src_gray, threshold_output, trsh, 255, THRESH_BINARY );
	/// Find contours
	findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Point2f>center( contours.size() );
	vector<float>radius( contours.size() );

	for( int i = 0; i < contours.size(); i++ )
	{
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
		minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
	}

	/// Draw polygonal contour + bonding rects + circles
	drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
		//circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
	}
//cout<<"ImgIdentify::apply_threshold:threshold="<<trsh<<" squares="<<contours.size()<<endl;
	return 0;
}
/*---------------------------------------------------------------------------*/
int ImgIdentify::scale_image(Mat& image, const float imgScaleFactor)
{
	cv::resize(image, image, cvSize(0, 0),imgScaleFactor,imgScaleFactor);
	return 0;
}
/*---------------------------------------------------------------------------*/
RPC_SRV_RESULT ImgIdentify::capture_camera_image(int video_device,int frmPixels,int frmLines,Mat& image)
{
	//capture image frame from /dev/video0 v4l2 device
	VideoCapture capture(video_device);//0 indicates /dev/video0
	capture.set(CV_CAP_PROP_FRAME_WIDTH,frmPixels);//1920
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,frmLines);//1080
	if(!capture.isOpened())
	{
		//cout << "Failed to connect to the camera." << endl;
		LOG_ERR_MSG("BRBOX:smarteye","ImgIdentify::capture_camera_image:failed to connect to camera");
		return RPC_SRV_RESULT_DEV_NOT_ACCESSIBLE;//-1;
	}
	capture >> image;
	if(image.empty())
	{
		//cout << "Failed to capture an image" << endl;
		LOG_ERR_MSG("BRBOX:smarteye","ImgIdentify::capture_camera_image:failed to capture image");
		return RPC_SRV_RESULT_FILE_EMPTY;//-1;
	}
	return RPC_SRV_RESULT_SUCCESS;//0;
}
/*---------------------------------------------------------------------------*/
RPC_SRV_RESULT ImgIdentify::identify_image_box(std::string imgPath)
{
	Mat image,src_gray;
	RPC_SRV_RESULT res;
	vector<vector<Point> > contours;

	//capture image
	res=capture_camera_image(0,1920,1080,image);
	if(res!=RPC_SRV_RESULT_SUCCESS)
		return res;

	/// Convert image to gray and blur it
	cvtColor( image, src_gray, CV_BGR2GRAY );
	blur( src_gray, src_gray, Size(3,3) );

	//apply threshold,and filter out un-wanted non-square stuff
	int LoopCount=0,scan_thresh=MIN_SCAN_THRESHOLD;
	RNG rng(12345);
	Mat contour_image;
	do
	{
		//cout<<"apply_threshold: scan_thresh = "<<scan_thresh<<endl;
		LOG_DEBUG_MSG_1_ARG(logflag,"BRBOX:smarteye","ImgIdentify::identify_image_box:scan_thresh=%d",scan_thresh);
		apply_threshold(src_gray,scan_thresh,rng,contour_image,contours);//filter-out unwanted non-square stuff
		scan_thresh+=20;//start from 100, and go upto 180
		//cout<<"apply_threshold: retured total squares = "<<contours.size()<<endl;
		LOG_DEBUG_MSG_1_ARG(logflag,"BRBOX:smarteye","ImgIdentify::identify_image_box:total_squares=%d",(int)contours.size());
	}while(contours.size()>MAX_SQARES_FILTER && LoopCount++<4);

	//if(contours.size()>MAX_SQARES_FILTER)
		//return RPC_SRV_RESULT_FAIL;//still there are many squares

	vector<Point> biggest_square;
	vector<Point> smaller_square;
	//find largest square
	find_largest_square(contours/*squares*/,biggest_square,1.5,1.8);//find outer white box of ovd/kvd identify pattern
	//find a smaller square which is 3times smaller than largest

//	if(find_relative_square(contours/*squares*/,biggest_square,smaller_square,3,30)==0)//; //+/- 20pixel tolerence
//		res=RPC_SRV_RESULT_SUCCESS;
//	else
//		res=RPC_SRV_RESULT_FAIL;//required image is not found

	if(find_relative_square_new(contours, biggest_square,smaller_square,3.15,3.25,0.8)==0)
		res=RPC_SRV_RESULT_SUCCESS;
	else
		res=RPC_SRV_RESULT_FAIL;//required image is not found



	//check if user requested to log the countour image
	if(imgPath!="")
		imwrite( imgPath, contour_image );

	return res;
}
/*---------------------------------------------------------------------------*/
RPC_SRV_RESULT ImgIdentify::identify_color_box(std::string imgPath,EJSON_SMARTEYE_IFACE_TYPE pattern)
{

	return RPC_SRV_RESULT_SUCCESS;
}
/*---------------------------------------------------------------------------*/
RPC_SRV_RESULT ImgIdentify::IdentifyPattern(EJSON_SMARTEYE_IFACE_TYPE pattern,std::string imgPath)
{
	switch(pattern)
	{
		case EJSON_SMARTEYE_IDPATTERN_IDENT:return identify_image_box(imgPath);//"/home/adav/tmp/adav-cam.jpg");
		case EJSON_SMARTEYE_IDPATTERN_RED  :return identify_color_box(imgPath,EJSON_SMARTEYE_IDPATTERN_RED);
		case EJSON_SMARTEYE_IDPATTERN_GREEN:
		case EJSON_SMARTEYE_IDPATTERN_BLUE :
		case EJSON_SMARTEYE_IDPATTERN_WHITE:
		default:break;
	}
	return RPC_SRV_RESULT_FAIL;
}
/*---------------------------------------------------------------------------*/

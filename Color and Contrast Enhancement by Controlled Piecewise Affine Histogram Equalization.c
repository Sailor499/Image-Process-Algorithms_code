void maxmin(Mat img_input,double *min,double *max)
{
	minMaxLoc(img_input,min,max,NULL,NULL);
}

void sort_data(Mat img_input,Mat F)
{
	
	cv::sort(F,F,CV_SORT_EVERY_ROW+CV_SORT_ASCENDING);

}


int inverse_cumulative_function(float Fu,Mat F)
{
	int pos;
	float *data = F.ptr<float>(0);

	if(Fu<0)
		return 0;

	pos = (int)ceil((double)Fu);
	pos = pos -1;

	return data[pos];
}



void affine_transformation(Mat img_input,Mat img_PEQ,float x0,float y0,float x1,float y1)
{
	float slope;
	int i,j,tmp;
	slope = (y1-y0)/(x1-x0);

	for(i = 0;i < img_input.rows;i++)
	{
		float *data = img_input.ptr<float>(i);
		float *out_data = img_PEQ.ptr<float>(i);
		for(j=0;j<img_input.cols;j++)
		{
			if(data[j]>=x0 && data[j]<=x1)
			{
				 tmp = (y0+slope*(data[j]-x0));
				 if(tmp > 255)
					 tmp = 255;
				 if(tmp < 0)
					 tmp = 0;
				 out_data[j] = tmp;
			}
		}
	}

}

void piecewise_transformation(Mat img_input,Mat img_PEQ,int N, float smin,float smax)
{
	float x0,x1,y0,y1,Fu,slope;
	double min,max;
	int k,col,row,dim;
	//Size size(1,img_input.cols*img_input.rows);
	//Mat F(size,CV_32FC1);
	Mat F;

	dim = img_input.cols*img_input.rows;
	img_input.reshape(0,1).copyTo(F);

	maxmin(img_input,&min,&max);
	sort_data(img_input,F);

	x0 = min;
	y0 = 0.0;
	
	
	for(k = 1;k <=N;k++)
	{
		Fu = (float)(dim*k)/(float)(N+1);//����Ҫ���ǵ�ԭ�㡣��ԭ��+N������ۻ��ֲ������ȷֳ�N��
		y1 = (255.0*k)/(N+1);
		x1 = inverse_cumulative_function(Fu,F);//������ķ�ʽ�������ۼƷֲ������ķ�����ֵ
		if (x1 > x0) 
		{
            slope = (y1 - y0) / (x1 - x0);//��ֵ���㣬�൱�ڰѷָ��xӳ�䵽[y0,y1]�ķ�Χ

            if (slope > smax)
                y1 = smax * (x1 - x0) + y0;

            if (slope < smin)
                y1 = smin * (x1 - x0) + y0;
		}

		affine_transformation(img_input,img_PEQ,x0,y0,x1,y1);
		x0 = x1;
        y0 = y1;

	}

	//����������ķָ��Ҷ�ֵ��СС��ԭʼͼ������ֵ����ô����Ҫ����һ��ӳ��
	if (x0 < max) 
	{
        y1 = 255.0;
        x1 = max;
        slope = (y1 - y0) / (x1 - x0);

        if (slope > smax)
            y1 = smax * (x1 - x0) + y0;

        if (slope < smin)
            y1 = smin * (x1 - x0) + y0;
		affine_transformation(img_input,img_PEQ,x0,y0,x1,y1);
	}

}



int main()
 {
	Mat img=imread("car.png",IMREAD_GRAYSCALE);
	Mat img_PEQ(img.size(),CV_8UC1);
	Mat img_float(img.size(),CV_32FC1);
	Mat img_float_PEQ(img.size(),CV_32FC1);
	Mat equalize_hist;
	
	

	img.convertTo(img_float,CV_32FC1);
	
	float smin = 0.5,smax = 1;
	int N = 10;
	piecewise_transformation(img_float,img_float_PEQ,N,smin,smax);
	convertScaleAbs(img_float_PEQ,img_PEQ);
	cv::equalizeHist(img,equalize_hist);
	
 // ����һ����Ϊ "��Ϸԭ��"����
	namedWindow("Original img",0);
	namedWindow("Modified img",0);
	namedWindow("equalize_hist img",0);

	

 // �ڴ�������ʾ��Ϸԭ��
	imshow("Original img",img);
	imshow("Modified img",img_PEQ);
	imshow("equalize_hist img",equalize_hist);

	imwrite("car_img_gray.bmp",img);
	imwrite("car_min0.5_max1.bmp",img_PEQ);
	imwrite("car_equalize_hist.bmp",equalize_hist);
	
 // �ȴ�6000 ms�󴰿��Զ��ر�
	
	waitKey(60000);
 }
 
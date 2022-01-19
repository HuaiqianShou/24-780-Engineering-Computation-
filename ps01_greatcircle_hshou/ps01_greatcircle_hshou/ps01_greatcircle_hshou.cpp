#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
# define M_PI           3.14159265358979323846  /* pi */
#pragma warning(disable:4996)

//Author: Huaiqian Shou 
//AndrewID: hshou
//Date:9/7/2021
//Course:24780-B
//Description of the code: Input latitude and longitude then calculate the great circle distance

double to_radians(double input)
{
	double output;
	output = input * M_PI / 180;

	return output;
}


double cal_dis(double latitude1, double latitude2, double longitude1, double longitude2)
{
	double la1_rad, la2_rad, lo1_rad, lo2_rad, distance, diff_latitude, a, angle_arc, diff_longitude;
	const double R = 6371.0;
	la1_rad = to_radians(latitude1);
	la2_rad = to_radians(latitude2);
	lo1_rad = to_radians(longitude1);
	lo2_rad = to_radians(longitude2);
	diff_latitude = abs(la1_rad - la2_rad);
	diff_longitude = abs(lo1_rad - lo2_rad);
	a = sin(diff_latitude / 2) * sin(diff_latitude / 2) + cos(la1_rad) * cos(la2_rad) * sin(diff_longitude / 2) * sin(diff_longitude / 2);
	angle_arc = 2 * atan2(sqrt(a), sqrt(1 - a));
	distance = R * angle_arc;
	return distance;
}

int main(void)
{
	
	double latitude1, latitude2, longitude1, longitude2, distance;
	using std::cout;
	using std::endl;
	cout << "24-780B Engineering Computation Prob Set 1 – Distance Calculator" << endl;
	cout << "   Enter latitude  of point 1 > ";
	scanf("%lf", &latitude1);

	cout << "   Enter longitude of point 1 > ";
	scanf("%lf", &longitude1);

	cout << "   Enter latitude  of point 2 > ";
	scanf("%lf", &latitude2);

	cout << "   Enter longitude of point 2 > ";
	scanf("%lf", &longitude2);
	cout << endl;

	distance = cal_dis(latitude1, latitude2, longitude1, longitude2);
	printf("Distance is >>> %lfkm",distance);

	return 0;

}




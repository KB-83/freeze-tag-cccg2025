#include <stdio.h>
#include <math.h>

// Global variables to store precomputed values
#define MAX_ANGLE  3142 // Number of angle values we precompute for `ring` and `distance` from 3.141 * 1000 + 1
#define MAX_R1 201 // from 0 to 200 to consider r1 goes from 0 to 1 by 0.005 steps
#define MAX_R2 201

#define ANGLE_D  1000 // Number of angle values we precomputed need to be devide by for use of `ring` and `distance`functions
#define R1_D 200 // devide factor of r1
#define R2_D 200 // devide factor of r1

// Global arrays to store precomputed values
float y_cache[MAX_ANGLE];  // Precomputed x(t) values
float distance_cache[MAX_R1][MAX_R2][MAX_ANGLE];  // Precomputed distance values
float ring_cache[MAX_ANGLE][MAX_R1];  // Precomputed ring values

// Function to calculate y(t) and store it in the global array
void precompute_y_values() {
	int i;
	for (i = 0; i < MAX_ANGLE; i++) {
		float t = (float)i / ANGLE_D; // t ranges from 0 to 3.141
		float phi = (1+sqrt(5))/2;
		y_cache[i] = pow(phi, 4) / (pow(phi, 3) + t) ;
	}
}

// Function to precompute distance values
void precompute_distances() {
	int i, j, k;
	for (i = 0 ; i < MAX_R1 ; i++) {
		for (j = 0; j < MAX_R2; j++) {
			for (k = 0; k < MAX_ANGLE; k++) {
				float a = (float)i/ R1_D;
				float b = (float)j/ R2_D;
				float angle = (float) k/ANGLE_D;
				if (pow(a, 2) + pow(b, 2) - 2* a * b * cos(angle) < 0)
				// It is impossible for the above function to be negative.
				// However, it's possible that ignoring some digits during computation could result in a negative value.
				// In such cases, the value is close to zero, so we can treat it as zero.
					distance_cache[i][j][k] =  0;
				else
				// The Euclidean distance between two points
				distance_cache[i][j][k] = sqrt(pow(a, 2) + pow(b, 2) - 2 * a * b * cos(angle));
			}
		}
	}
}

float y(float t) {
	return y_cache[(int)(t*ANGLE_D)];
}

float distance(float a, float b, float angle) {
	return distance_cache[(int)(a*R1_D)][(int)(b*R2_D)][(int)(angle*ANGLE_D)];
}

// The time required to activate a crown by two activate robots in a outer corner
float crown(float angle, float r) {
	return y_cache[(int)(ANGLE_D*angle)] * (1-r) + angle;
}

// time of (two or four) crowns algorithm
float time_2_or_4(float r_1, float r_2, float r_3, double miu_12, double miu_13) {
	// We find the worst case of beta
	float left, right, a, b, x, t_2 = 222, t_4 = 444, beta, t_24, t_24_worst = 0,inloop = 0;
	for (beta = 0; beta <= 3; beta = beta + 0.01) {
		// 't_2' is time of two crowns algorithm with respect to beta
		t_2 = 1 + crown(3.14159 - beta, r_2);

		// We find the best value of the angle x by a binary search, the interval of binary search is [left, right]
		// Using the variable `inloop` to avoid handling loop maintenance when `fabs(b - a) > 0.001` does not occur due to discrete computations.
		left = 0; right = 6.28; a = 1000; b = 2000,inloop = 0;
		while (fabs(b - a) > 0.001 && inloop < 50) {
			x = 0.5 * (left + right);
			// 'a' is the time taken to activate two crowns, and 'b' is the time taken to activate the other two crowns
			a = r_2 + distance(r_2, r_1, miu_12) + distance(r_1, r_3, beta) + (1 - r_3) + crown(x, r_3);
			b = r_2 + distance(r_2, 0.5*(1+r_3), 3.14159 + beta - miu_12) + (2 + 0.5 *  sqrt(5)) * (1 - r_3) + (3.14159 - x);
			if (a < b)  left = x;  else  right = x;
			inloop++;
		}
		// 't_4' represents the time taken to activate all four crowns
		t_4 = fmax(a,b);
		// 't_24' selects the faster algorithm between the two-crown algorithm (with respect to beta) and the four-crown algorithm (with respect to beta)
		t_24 = fmin(t_2, t_4);
		// 't_2040' selects the time for the two- or four-crown algorithm (with respect to beta) from all possible values of beta
		t_24_worst = fmax(t_24_worst, t_24);
	}
	return t_24_worst;
}

// Time of two crowns algorithm with respect to r_3
float time_2(float r_1, float r_2, float r_3, double miu_12, double miu_13) {
	float left = 0, right = 3.14, a = 0, b = 2025, x,inloop = 0;
	// We find the optimal value of the angle x using binary search, where the search interval is [left, right]
	while (fabs(b - a) > 0.001 && inloop < 50) {
		x = 0.5 * (left + right);
		// a is the time taen to activate a crown, and b is the time taken to activate another crown
		a = r_3 + (1 + y(x)) * (1 - r_3) + x + 2 * (r_3 - r_2);
		b = r_3 + (1 + y(2 * 3.1416 - x)) * (1 - r_3) + (2 * 3.1416 - x);
		if (a < b)  left = x;  else  right = x;
		// Using the variable `inloop` to avoid handling loop maintenance when `fabs(b - a) > 0.001` does not occur due to discrete computations.
		inloop++;
	}
	return fmax(a,b);
}

// Time of three crowns algorithm with respect to r_3
float time_3(float r_1, float r_2, float r_3, double miu_12, double miu_13) {
	float a, b, q, p = 2025, x;
	for (x = 0; x <= 3.14; x = x + 0.01) {
		// 'a' is the time taken to activate two crowns, and 'b' is the time taken to activate another crown
		a = r_1 + distance(r_1, r_2, miu_12) + fabs(r_3-r_2) + (1 - r_3) + crown(x, r_3);
		b = r_1 + distance(r_1, r_3, miu_12 - x)  + (1 - r_3) + crown(2 * 3.14159 - (2 * x), r_3);
		// 'q' is the total time for all crowns
		q = fmax(a, b);
		// 'p' is the total time for the best value of the angle x
		p = fmin(p, q);
	}
	return p;
}

// The time taken by the algorithms using robot p_3 before activating the crowns
float time_robot3(float r_1, float r_2, float r_3, double miu_12, double miu_13) {

	float left = 0, right = 3.14, a =0, b = 2025, x, f_1 = 2025, f_2 = 2025, f, miu_23, m_23,inloop = 0;

	// Let D be the diameter of the circle that contains p_1, which divides the circle into two half-circles.
    // miu_23 is the angle between p_2 and p_3 when these two points lie on different half-circles.
    // m_23 is the angle between p_2 and p_3 when these two points lie on the same half-circle
	if (miu_12 > miu_13) m_23 = miu_12 - miu_13; else m_23 = miu_13 - miu_12;
	if(miu_12 + miu_13 < 3.14) miu_23 = miu_12 + miu_13; else  miu_23 = 2 * 3.14159 - miu_12 - miu_13;


	// There are 10 algorithms depending on the distributions of p_1 , p_2 and p_3, all of which use robot p_3.
	// The first 5 algorithms are used when the angle between p_2 and p_3 is miu_12 + miu_13. The fastest algorithm among these 5 has a time of 'f_1'
	// The last 5 algorithms are used when the angle between p_2 and p_3 is |miu_12 - miu_13|. The fastest algorithm among these 5 has a time of 'f_2'
	// Note that since we return the maximum of 'f_1' and 'f_2', both cases of the angle between p_2 and p_3 are considered

	 // Finding the optimal angle x in the four-crown algorithm, starting from p_1 (followed by p_2 and p_3)
	a = 0; b = 2025; left = 0; right = 3.14,inloop = 0;
	while (fabs(b - a) > 0.001 && inloop < 50) {
		x = 0.5 * (left + right);
		a = r_1 + distance(r_1, r_2, miu_12) + distance(r_2, r_3, 0.5 * (3.14159 - miu_23)) + (1 - r_3) + crown(x, r_3);
		b = r_1 + distance(r_1, r_3, miu_13) + distance(r_3, r_3, 0.5 * (3.14159 - miu_23)) + (1 - r_3) + crown(3.14159 - x, r_3);
		if (a < b)  left = x;  else  right = x;
		inloop++;
	}
	f_1 = fmin(f_1, fmax(a,b));

	// Finding the optimal angle x in the four-crown algorithm, starting from p_2 (followed by p_1 and p_3)
	a=0; b = 2025; left = 0; right = 3.14,inloop = 0;
	while (fabs(b - a) > 0.001 && inloop < 50) {
		x = 0.5 * (left + right);
		a = r_2 + distance(r_2, r_1, miu_12) + distance(r_1, r_3, 0.5 * (3.14159 - miu_13)) + (1 - r_3) + crown(x, r_3);
		b = r_2 + distance(r_2, r_3, miu_23) + distance(r_3, r_3, 0.5 * (3.14159 - miu_13)) + (1 - r_3) + crown(3.14159 - x, r_3);

		if (a < b)  left = x;  else  right = x;
		inloop++;
	}
	f_1 = fmin(f_1, fmax(a,b));

	// Finding the optimal angle x in the four-crown algorithm, starting from p_3 (followed by p_1 and p_2)
	a=0; b = 2025; left = 0; right = 3.14,inloop = 0;
	while (fabs(b - a) > 0.001 && inloop < 50) {
		x = 0.5 * (left + right);
		a = r_3 + distance(r_3, r_1, miu_13) + distance(r_1, r_3, 0.5 * (3.14159 - miu_12)) + (1 - r_3) + crown(x, r_3);
		b = r_3 + distance(r_3, r_2, miu_23) + distance(r_2, r_3, 0.5 * (3.14159 - miu_12)) + (1 - r_3) + crown(3.14159 - x, r_3);
		if (a < b)  left = x;  else  right = x;
		inloop++;
	}
	f_1 = fmin(f_1, fmax(a,b));

	// The time of the three-crown algorithm, starting from p_2 (followed by p_3)
	// Note that in this case, the width of the crowns is 1-(r_1)
	f_1 = fmin(f_1, time_3(r_2, r_3, r_1, miu_23, 1000));

	// The time of three crowns algorithm,  starting from p_1 (followed by p_3)
	// Note that in this case, the width of the crowns is 1-(r_2)
	f_1 = fmin(f_1, time_3(r_1, r_3, r_2, miu_13, 1000));

   // Here, 'f_1' represents the time of the fastest algorithm among
   // the five algorithms mentioned above, using robot p_3, where the angle between p_2 and p_3 is 'miu_12 + miu_13'
   // Now going to calculate 'f_2' for other case of angle between p_2 and p_3

	// Finding the optimal angle x in the four-crown algorithm, starting from p_3 (followed by p_1 and p_2)
	a = 0; b = 2025; left = 0; right = 3.14; inloop = 0;
	while (fabs(b - a) > 0.001 && inloop < 50) {
		x = 0.5 * (left + right);
		a = r_3 + distance(r_3, r_1, miu_13) + distance(r_1, r_3, 0.5 * (3.14159 - miu_12)) + (1 - r_3) + crown(x, r_3);
		b = r_3 + distance(r_3, r_2, m_23) + distance(r_2, r_3, 0.5 * (3.14159 - miu_12)) + (1 - r_3) + crown(3.14159 - x, r_3);
		if (a < b)  left = x;  else  right = x;
		inloop++;
	}
	f_2 = fmin(f_2, a);

	// Finding the optimal angle x in the four-crown algorithm, starting from p_1 (followed by p_2 and p_3)
	a = 0; b = 2025; left = 0; right = 3.14; inloop = 0;
	while (fabs(b - a) > 0.001 && inloop < 50) {
		x = 0.5 * (left + right);
		a = r_1 + distance(r_1, r_2, miu_12) + distance(r_2, r_3, 0.5 * (3.14159 - m_23)) + (1 - r_3) + crown(x, r_3);
		b = r_1 + distance(r_1, r_3, miu_13) + distance(r_3, r_3, 0.5 * (3.14159 - m_23)) + (1 - r_3) + crown(3.14159 - x, r_3);
		if (a < b)  left = x;  else  right = x;
		inloop++;
	}
	f_2 = fmin(f_2, a);

	// Finding the optimal angle x in the four-crown algorithm, starting from p_2 (followed by p_1 and p_3)
	a=0; b = 2025; left = 0; right = 3.14; inloop = 0;
	while (fabs(b - a) > 0.001 && inloop < 50) {
		x = 0.5 * (left + right);
		a = r_2 + distance(r_2, r_1, miu_12) + distance(r_1, r_3, 0.5 * (3.14159 - miu_13)) + (1 - r_3) + crown(x, r_3);
		b = r_2 + distance(r_2, r_3, m_23) + distance(r_3, r_3, 0.5 * (3.14159 - miu_13)) + (1 - r_3) + crown(3.14159 - x, r_3);
		if (a < b)  left = x;  else  right = x;
		inloop++;
	}
	f_2 = fmin(f_2, a);

	// Finding the optimal angle x in the four-crown algorithm, starting from p_3 (followed by p_1 and p_2)
	a = 0; b = 2025; left = 0; right = 3.14 ; inloop = 0;
	while (fabs(b - a) > 0.001 && inloop < 50) {
		x = 0.5 * (left + right);
		a = r_3 + distance(r_3, r_1, miu_13) + distance(r_1, r_3, 0.5 * (3.14159 - miu_12)) + (1 - r_3) + crown(x, r_3);
		b = r_3 + distance(r_3, r_2, m_23) + distance(r_2, r_3, 0.5 * (3.14159 - miu_12)) + (1 - r_3) + crown(3.14159 - x, r_3);
		if (a < b)  left = x;  else  right = x;
		inloop++;
	}
	f_2 = fmin(f_2, a);

	// The time of three crowns algorithm starting from p_2 (followed by p_3)
	// Note that in this case, the width of crowns is 1-(r_1)
	f_2 = fmin(f_2, time_3(r_2, r_3, r_1, m_23, 1000));

	// The time of three crowns algorithm starting from p_1 (followed by p_3)
	// note that in this case, the width of crowns is 1-(r_2)
	f_2 = fmin(f_2, time_3(r_1, r_3, r_2, miu_13, 1000));


	// Considering both cases for the angle between p_2 and p_3
	f = fmax(f_1, f_2);
	return f;
}

// Finding the fastest algorithm
// If the times of some algorithms are less than 4.27, we can disregard the other algorithms, as our goal is to stay within the upper bound of 4.27.
float best_time(float r_1, float r_2, float r_3, double miu_12, double miu_13) {
	float time;
	time = time_2(r_1, r_2, r_3, miu_12, miu_13);
	//if (time < 4.27) return time;
		time = fmin(time, time_3(r_1, r_2, r_3, miu_12, miu_13));
	//if (time < 4.27) return time;
	    time = fmin(time, time_robot3(r_1, r_2, r_3, miu_12, miu_13));
	//if (time < 4.27) return time;
	   time = fmin(time, time_2_or_4(r_1, r_2, r_3, miu_12, miu_13));
	return time;
}

int main() {
	precompute_y_values();
	precompute_distances();

    float r_1 = 1, r_2 = 1, r_3 = 1, miu_12, miu_13, makespan = 0, t;
    for(r_1 = 0; r_1 <= 1; r_1 = r_1 + 0.005) {
    	for(r_2 = r_1; r_2 <= 1; r_2 = r_2 + 0.005) {
    		for(r_3 = r_2; r_3 <= 1; r_3 = r_3 + 0.005) {
    			for(miu_12 = 0; miu_12 <= 3.15; miu_12 = miu_12 + 0.01) {
    				for(miu_13 = 0; miu_13 <= 3.15; miu_13 = miu_13 + 0.01) {
   						t = best_time(r_1, r_2, r_3, miu_12, miu_13);
    					if (t > makespan) {
    						// log of code :
    						printf("%.3f  %.3f  %.3f  %.2f  %.2f  %.4f \n", r_1, r_2, r_3, miu_12, miu_13, t);
    						makespan = t;
    					}
					}
   				}
   			}
		}
	}
	printf("%.4f", makespan);
}

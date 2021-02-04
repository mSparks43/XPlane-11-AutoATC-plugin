/*
*****************************************************************************************
*        COPYRIGHT � 2020 Mark Parker/mSparks


GNU Lesser General Public License v3.0
Permissions of this copyleft license are conditioned on making available complete source code of
licensed works and modifications under the same license or the GNU GPLv3. Copyright and license 
notices must be preserved. Contributors provide an express grant of patent rights. 
However, a larger work using the licensed work through interfaces provided by the licensed work may 
be distributed under different terms and without source code for the larger work.
*****************************************************************************************
*/
#include "Simulation.h"
#include "scppnt/trisolve.h"
#include <math.h>
//Filter simulation to remove lat long alt -> meter conversion noise, x,y filter
Simulation::Simulation(double rate){
    double aelem[16] = {
				1, 0, rate, 0,
				0, 1, 0, rate,
				0, 0, 1, 0,
				0, 0, 0, 1
				};
    Matrix<double> A(aelem,4,4);
    this->A=A;
    double belem[16] = {
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
				};
    Matrix<double> B(belem,4,4);
    this->B=B;
    double helem[16] = {
				1, 0, 1, 0,
				0, 1, 0, 1,
				0, 0, 0, 0,
				0, 0, 0, 0
				};
    Matrix<double> H(helem,4,4);
    this->H=H;
    double qelem[16] = {
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0.01, 0,
				0, 0, 0, 0.01
				};
    Matrix<double> Q(qelem,4,4);
    this->Q=Q;
    double relem[16] = {
				0.1, 0, 0, 0,
				0, 0.1, 0, 0,
				0, 0, 0.1, 0,
				0, 0, 0, 0.1
				};
    Matrix<double> R(relem,4,4);
    this->R=R;
    double xelem[4] = {-36.23380577242374, 25.750152318401334, -5338.888247605355, 3794.210217903644};
    Matrix<double> last_x(xelem,4,1);
    this->last_x=last_x;
    double pelem[16] = {
				8.384082438396048E-7, 0.0, 7.614271232688775E-5, 0.0, 
                0.0, 8.384082438396048E-7, 0.0, 7.614271232688775E-5, 
                7.614271232688775E-5, 0.0, 0.016006051783141895, 0.0, 
                0.0, 7.614271232688775E-5, 0.0, 0.016006051783141895
				};
    Matrix<double> last_P(pelem,4,4);
    this->last_P=last_P;
    double ielem[16] = {
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
				};
    Matrix<double> I(ielem,4,4);
    this->I=I;
    xPos=0;
    yPos=0;

}

void Simulation::frame(){
    double cur_xPos = xPos;
	double cur_yPos = yPos;
    double velX = cur_xPos - last_x[0][0];
	double velY = cur_yPos - last_x[1][0];
    double melem[4] = {cur_xPos, cur_yPos, velX, velY};

    Matrix<double> measurement(melem,4,1);
    //double controlelem[4] = {0, 0, 0, 0};
    //Matrix<double> control(controlelem,4,1);
    // prediction
    
    Matrix<double> x = A * last_x; //+B*control = +0
    
    Matrix<double> P = ((A * last_P)*(transpose(A)))+Q;
    
    // correction
    Matrix<double> S = ((H * P)*(transpose(H)));
    S += R;
    Matrix<double> K = (P * transpose(H)) * (inverse(S));
    Matrix<double> y = measurement;
    y -=(H*x);

    last_x=x+(K*y);
    last_P=(I-(K*H))*P;
    
}
double Simulation::getX(){
    return last_x[0][0];
}

double Simulation::getY(){
     return last_x[1][0];
}
Matrix<double> Simulation::inverse(Matrix<double> B){
    double LU[4][4];
    int piv[4];
    for(int m=0;m<4;m++){
        piv[m]=m;
        for (int n=0;n<4;n++)
            LU[m][n]=B[m][n];
    }
    int pivsign = 1;
    double LUrowi[4];
    double LUcolj[4];
    double s = 0.0;
    int p=0;
    int kmax=0;
    //for(int z=0;z<4;z++)
     //   printf("nframe {%f,%f,%f,%f} %f %f:%f %f\n",LU[z][0],LU[z][1],LU[z][2],LU[z][3],xPos,yPos,getX(),getY());
    for (int j = 0; j < 4; j++) {
        //for(int z=0;z<4;z++)
        //    printf("%d cframe {%f,%f,%f,%f} %f %f:%f %f\n",j,LU[z][0],LU[z][1],LU[z][2],LU[z][3],xPos,yPos,getX(),getY());
        // Make a copy of the j-th column to localize references.
        for (int i = 0; i < 4; i++) {
         LUcolj[i] = LU[i][j];
        } 
        // Apply previous transformations.
        for (int i = 0; i < 4; i++) {
            for(int nm=0;nm<4;nm++)
                LUrowi[nm] = LU[i][nm];

         // Most of the time is spent in the following dot product.

         kmax = std::min(i,j);
         s = 0.0;
         for (int k = 0; k < kmax; k++) {
            s += LUrowi[k]*LUcolj[k];//s += B[i][k]*B[k][j];
         }

         LU[i][j] = LUcolj[i] -= s;
      }
      // Find pivot and exchange if necessary.

      p = j;
      for (int i = j+1; i < 4; i++) {
         if (abs((double)LUcolj[i]) > abs((double)LUcolj[p])) {
            p = i;
         }
      }
      if (p != j) {
         for (int k = 0; k < 4; k++) {
            double t = LU[p][k]; 
            LU[p][k] = LU[j][k]; 
            LU[j][k] = t;
         }
         int k = piv[p]; 
         piv[p] = piv[j]; 
         piv[j] = k;
         pivsign = -pivsign;
      }
        if (j < 4 & LU[j][j] != 0.0) {
         for (int i = j+1; i < 4; i++) {
            LU[i][j] =  (LU[i][j]/LU[j][j]);
         }
      }
      
    }
   //for(int z=0;z<4;z++)
     //   printf("iframe {%f,%f,%f,%f} %f %f:%f %f\n",LU[z][0],LU[z][1],LU[z][2],LU[z][3],xPos,yPos,getX(),getY());
    
   double xelem[16] = {
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0
				};
   Matrix<double> X(xelem,4,4);
   for (int i = 0; i < 4; i++) {
            for (int j = 0; j <= 3; j++) {
               X[i][j] = I[piv[i]][j];
            }
   }
//printf("xframe {%f,%f,%f,%f} %f %f:%f %f\n",X[1][0],X[1][1],X[1][2],X[1][3],xPos,yPos,getX(),getY());
   // Solve L*Y = B(piv,:)
   for (int k = 0; k < 4; k++) {
      for (int i = k+1; i < 4; i++) {
         for (int j = 0; j < 4; j++) {
            X[i][j] -= X[k][j]*LU[i][k];
         }
      }
   }
   //printf("x2frame {%f,%f,%f,%f} %f %f:%f %f\n",X[1][0],X[1][1],X[1][2],X[1][3],xPos,yPos,getX(),getY());
   // Solve U*X = Y;
   for (int k = 3; k >= 0; k--) {
      for (int j = 0; j < 4; j++) {
         X[k][j] = (X[k][j]/LU[k][k]);
      }
      for (int i = 0; i < k; i++) {
         for (int j = 0; j < 4; j++) {
            X[i][j] =  X[i][j]-(X[k][j]*LU[i][k]);
         }
      }
   }
   //printf("x2frame {%f,%f,%f,%f} %f %f:%f %f\n",X[1][0],X[1][1],X[1][2],X[1][3],xPos,yPos,getX(),getY());
   return X;
}


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
#define SCPPNT_NO_DIR_PREFIX
#include "scppnt/vec.h"
#include "scppnt/cmat.h"
#include "scppnt/matop.h"
#include "scppnt/transv.h"
#include "scppnt/region2d.h"
#include "scppnt/scppnt_error.h"
using namespace SCPPNT;

class Simulation{
    private:
    Matrix<double> A;
    Matrix<double> B;
    Matrix<double> H;
    Matrix<double> Q;
    Matrix<double> R;
    Matrix<double> I;
    
    
    public:
    Simulation(double rate);
    Matrix<double> inverse(Matrix<double> B);
    double getX();
    double getY();
    void frame();
    double xPos;
    double yPos;
    Matrix<double> last_x;
    Matrix<double> last_P;
};

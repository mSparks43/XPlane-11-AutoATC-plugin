
#define SCPPNT_NO_DIR_PREFIX
#include "scppnt/vec.h"
#include "scppnt/cmat.h"
#include "scppnt/matop.h"
#include "scppnt/transv.h"
#include "scppnt/region2d.h"

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
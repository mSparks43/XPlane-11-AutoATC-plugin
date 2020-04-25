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
typedef int i;       //Save space by using 'i' instead of 'int'
typedef float f;     //Save even more space by using 'f' instead of 'float'

   //Define a vector class with constructor and operator: 'v'
   struct v{
      f x,y,z;  // Vector has three float attributes.
      v operator+(v r){return v(x+r.x,y+r.y,z+r.z);} //Vector add
      v operator*(f r){return v(x*r,y*r,z*r);}       //Vector scaling
      f operator%(v r){return x*r.x+y*r.y+z*r.z;}    //Vector dot product
      v(){}                                  //Empty constructor
      v operator^(v r){return v(y*r.z-z*r.y,z*r.x-x*r.z,x*r.y-y*r.x);} //Cross-product
      v(f a,f b,f c){x=a;y=b;z=c;}            //Constructor
      v operator!(){return *this*(1 /sqrt(*this%*this));} // Used later for normalizing
      f operator/(v r){

          return sqrt(x*x+y*y+z*z);
        } //get magnitude
      v operator-(v r){
          return v(x-r.x,y-r.y,z-r.z);
          }  //Vector subtract
   };
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
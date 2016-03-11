# back-face-culling
Convert an OBJ file to SVG while removing hidden surfaces using back face culling.

User gives an obj file as input. Each face in the obj file is either a 3D triangle or a 3D quadrilateral.
Apply rotation in the object space, as specified the user. Now apply the back face culling and project the object 
on the view-port whose dimensions are also supplied by the user. You can use the Painter's Algorithm for this. 
After projection, the entire image should be just fitting either the width or the height of the view-port 
with a 10-pixel margin on all sides. Save the result as an SVG file and view it in Firefox or some other web browser.
   
User input:   
obj file   
view-port dimensions   
camera coordinates   
angles of rotation about x-, y-, or/and z-axis. The axes should be local w.r.t. the object, considering   
(0,0,0) as the "center" of the 3D bounding box containing the object.   
Examples: cube.obj & cube.svg    
   
   
Compile as: g++ backFaceCulling.cpp   
Execute as: ./a.out   
   
Inputs needed:   
objfile name(present in same directory)   
height of viewport   
width of viewport   
x coordinate of camera   
y coordinate of camera   
z coordinate of camera   
angle of rotation about x-axis   
angle of rotation about y-axis   
angle of rotation about z-axis   
   
Output:   
Output is written to a file called output.svg   
   
Notes:   
1. angle of rotation is between 0 and 180 degrees. enter 0 if not needed.   
2. z coordinate of camera should be positive and greater than the maximum z coordinate in the objfile for proper projections.   
3. output_file_description.txt describes the input given for various svg outputs for cube.obj and f16.obj   
4. Rotation is done about x-axis then y-axis then z-axis.   
   
   
For ease of execution, I have added an extra file named "input" which can be used to indirect input instead of entering the values every time. Just edit the values in the file to change the output.   
Execute as: ./a.out < input    
    
1. First line contains name of objfile.   
2. Second line contains height and width of viewport.   
3. Third line contains x,y and z coordinates of camera.   
4. Fourth line contains angles of rotaition about x,y and z axes respectively.   
   

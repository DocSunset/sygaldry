CUT = 1000;

KeyWidth = 6;
KeySpacing = 117.5 / 10; //KeyWidth + KeyGapWidth;
KeyGapWidth = KeySpacing - KeyWidth;

KeyTravel = 2;
MinSensorDistance = 1.5;
MinKeyHeight = 1;
MaxKeyHeight = MinKeyHeight + KeyTravel;
SensorHeight = 4.5;
SensorWidth = 4.5;
SensorLength = 7;
SensorOffset = 4.5;
SensorGap = 5;

SlotWidth = 2.2;
SlotHeight = 7.5;
NSlots = 33/3;

PcbHeight = 20;
PcbStopThickness = 2;
HookHeight = MaxKeyHeight + MinSensorDistance + SensorHeight;
HookLength = 21;
HookDrop = 20;
HookArm = 2;
HookThickness = SlotWidth-0.1;
HookRetainerHeight = 1;
HookRetainerThickness = 1;

RailHeight = 15;
RailLength = KeySpacing*(NSlots-1);
echo(RailLength);
RailThickness = 2;
RailArea = RailThickness * 2 + 3 * HookRetainerThickness;

TotalLength = RailArea + HookLength;

echo(TotalLength);
assert(TotalLength < 33);

// from hook print aligned perspective

ShelfWidth = 10;
ShelfLength = 10;
SelfThickness = 10;
ShelfRetainerHeight = 5;

module rail()
{
    difference()
    {
        cube([RailLength, RailHeight, RailThickness]);
        // make iterative
        for (i = [0:1:NSlots]) {
            translate([-SlotWidth/2+i*KeySpacing, -1, -1])
                cube([SlotWidth, SlotHeight+1, CUT]);
            translate([KeyGapWidth/2-0.5+i*KeySpacing, -1, -1])
                cube([KeyWidth+1, MaxKeyHeight+1, CUT]);
        }
    }
}

module hook()
{
    difference()
    {
        union()
        {
            // bottom bar
            cube([HookLength, HookArm, HookThickness]);
            // PCB support
            translate([SensorOffset,0,0])
                cube([SensorLength, HookHeight, HookThickness]);
            // PCB stops
            translate([-PcbStopThickness,0,0])
                cube([PcbStopThickness, HookHeight+1.6, HookThickness]);
            translate([-PcbStopThickness,HookHeight+1.6,0])
                cube([PcbStopThickness+.8, 1.6, HookThickness]);
            translate([PcbHeight,0,0])
                cube([PcbStopThickness, HookHeight+1.6, HookThickness]); 
            translate([PcbHeight-0.8,HookHeight+1.6,0])
                cube([PcbStopThickness+.8, 1.6, HookThickness]);
            // slot joint
            translate([HookLength,0,0])
                cube([RailArea, SlotHeight-0.1, HookThickness]);
            // retainer walls
            translate([HookLength,0,0])
                cube([HookRetainerThickness, SlotHeight, HookThickness+HookRetainerHeight]);
            translate([HookLength+HookRetainerThickness+RailThickness+0.1,0,0])
                cube([HookRetainerThickness-0.2, SlotHeight, HookThickness+HookRetainerHeight]);
            translate([HookLength+HookRetainerThickness*2+RailThickness*2,0,0])
                cube([HookRetainerThickness, SlotHeight, HookThickness+HookRetainerHeight]);
        }
        //translate([-HookArm, HookArm, -1]) cube([HookLength, HookDrop, CUT]);
    }
}

module aligner()
{
    AlignerWidth = SensorWidth*2 + SensorGap + 2;
    difference()
    {
        union() {
            cube([AlignerWidth, PcbHeight + 4, 2]);
            cube([AlignerWidth, 2, SensorHeight + 2]);
            translate([0,PcbHeight+2,0])
            cube([AlignerWidth, 2, SensorHeight + 2]);
            translate([0,2,SensorHeight+1.6])
                cube([AlignerWidth, 0.2, 0.4]);
            translate([0,PcbHeight+2-0.2,SensorHeight+1.6])
                cube([AlignerWidth, 0.2, 0.4]);
            translate([1+SensorWidth,0,0]) cube([SensorGap,PcbHeight,SensorHeight]);
        }
        translate([1+SensorWidth + SensorGap,SensorOffset+2,0])
            cube([SensorWidth, SensorLength, SensorHeight]);
        translate([1,SensorOffset+2,0])
            cube([SensorWidth, SensorLength, SensorHeight]);
    }
}


module frame()
{
    keyboard_x = 402;
    left_cheek_x  = 26;
    right_cheek_x = 23;
    full_x = left_cheek_x + keyboard_x + right_cheek_x; // == 451
    cheek_z = 9;
    key_z = 4;
    frame_y = 24;
    frame_z = 24;
    
    module keyboard_space()
    {
        full_cheek_z = 19;
        full_cheek_y = 165;
        key_y = 84;
        translate([0,-key_y,-cheek_z]) union()
        {
        cube([left_cheek_x,full_cheek_y,full_cheek_z]);
        translate([left_cheek_x,0,0]) cube([keyboard_x,key_y,17]);
        translate([left_cheek_x,0,0]) cube([keyboard_x,107,12]);
        translate([left_cheek_x + keyboard_x,0,0]) 
            cube([right_cheek_x,full_cheek_y,full_cheek_z]);
        }
    }
    
    //keyboard_space();

    module _frame()
    {
    ang = 60;
    //hyp = sqrt(frame_y*frame_y + 100);
    difference()
    {
        // main bar
        cube([full_x,frame_y,frame_z]);
        // cut for left cheek
        cube([left_cheek_x,999,cheek_z]);
        // left screw hole
        translate([left_cheek_x/2,frame_y-7,cheek_z]) cylinder(h=999,d=3);
        translate([left_cheek_x/2,frame_y-7,cheek_z+3]) cylinder(h=999,d=6);
        
        // cut for right cheek
        translate([left_cheek_x + keyboard_x,0,0]) cube([right_cheek_x,999,cheek_z]);
        // cut for cable on right
        translate([full_x-5,0,cheek_z]) cube([5,999,5]);
        // right screw hole
        translate([full_x-right_cheek_x/2,frame_y-7,cheek_z]) cylinder(h=999,d=3);
        translate([full_x-right_cheek_x/2,frame_y-7,cheek_z+3]) cylinder(h=999,d=6);
        
        translate([0,0,key_z]) rotate([ang,0,0]) cube([999,999,999]);
        translate([left_cheek_x,10,0]) cube([keyboard_x,5,RailHeight]);
        translate([left_cheek_x,9,0])  cube([keyboard_x,1,9]);
        translate([left_cheek_x,10,0]) cube([keyboard_x,999,RailHeight-3]);
        translate([left_cheek_x,18,0]) cube([keyboard_x,999,frame_z-1]);
    }
    }
    _frame();
}

rail();
//hook();
//aligner();
//frame();

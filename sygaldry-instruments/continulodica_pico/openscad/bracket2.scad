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
HookLength = 23;
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
                cube([PcbStopThickness, HookHeight+2, HookThickness]);
            translate([PcbHeight,0,0])
                cube([PcbStopThickness, HookHeight+2, HookThickness]); 
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

module shelf()
{
}

//rail();
hook();
//aligner();

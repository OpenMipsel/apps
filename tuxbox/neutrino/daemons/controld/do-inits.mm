!transformation

!begin []
//"// This file was automatically generated from ",
//sourcenames,
//", do not edit\n",
source.child;

vendor []
!replace 
"avs_vendor_settings ",
  self.@name,
  "_settings = {\n",
  "    // v1      a1           v2                     a2       v3     a3 fblk\n",

  self.r.child[@mode == "dvb"],
  ", // DVB\n",
  self.r.child[@mode == "scart"],
  " // Scart\n};\n";

operating-mode []
!replace 
"  {{",
  join(self.r.child[?tv-format].child[?v1].@value, ","),
  "}, ",
  self.r.child[?a1],
  ", {",
  self.r.child[?tv-format],
  "}, ",
  self.r.child[?a2], ", {",
  join(self.r.child[?tv-format].child[?v3].@value, ","),
  "}, ",
  self.r.child[?a3], ", ",
  self.r.child[?fblk],
"}";

v1|v2|a1|a2|a3|fblk []
!replace self.@value;

tv-format []
!replace
(self.r.left & ","), 
"{",
  self.r.child[@format == "cvbs"],
  ",", 
  self.r.child[@format == "svideo"],
  "}"
  ; 

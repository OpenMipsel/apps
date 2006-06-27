!transformation

!begin []
  "#		    v1      a1           v2                   a2       v3   a3 fblk\n",
source.child;

vendor []
!replace 
  self.@name,
  "_scart: ",
  "                     ".substring(self.@name.length(),6),
  self.r.child[@mode == "scart"], "\n",
  self.@name,
  "_dvb:   ",
  "                     ".substring(self.@name.length(),6),
  self.r.child[@mode == "dvb"], "\n"
  ;

operating-mode []
!replace 
"  {",
  join(self.r.child[?tv-format].child[?v1].@value, " "),
  "} ",
  self.r.child[?a1],
  " {",
  self.r.child[?tv-format],
  "} ",
  self.r.child[?a2], " {",
  join(self.r.child[?tv-format].child[?v3].@value, " "),
  "} ",
  self.r.child[?a3], " ",
  self.r.child[?fblk]
;

v1|v2|a1|a2|a3|fblk []
!replace self.@value;

tv-format []
!replace
(self.r.left & " "), 
"{",
  self.r.child[@format == "cvbs"],
  " ", 
  self.r.child[@format == "svideo"],
  "}"
  ; 

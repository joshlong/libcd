#ifndef DISC_H
#define DISC_H

class CDisc {
	
	public:
  		virtual unsigned GetTrackCount() const = 0;
  		virtual unsigned long GetStartFrame(unsigned track) const = 0;
  		virtual unsigned long GetFrames(unsigned track) const = 0;
  		virtual ~CDisc() { };
};

#endif 

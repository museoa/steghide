#ifndef SH_SAMPLEOCCURENCE_H
#define SH_SAMPLEOCCURENCE_H

// decalred here to prevent circulating #includes
class Vertex ;

class SampleOccurence {
	public:
	SampleOccurence (Vertex *v, unsigned short i)
		: TheVertex(v), Index(i) {} ;

	Vertex *getVertex (void) const
		{ return TheVertex ; } ;

	void setVertex (Vertex* v)
		{ TheVertex = v ; } ;

	unsigned short getIndex (void) const
		{ return Index ; } ;

	void setIndex (unsigned short i)
		{ Index = i ; } ;

	private:
	Vertex *TheVertex ;
	unsigned short Index ;
} ;

#endif // ndef SH_SAMPLEOCCURENCE_H

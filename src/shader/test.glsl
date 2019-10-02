#version 320 es
#define TAILLONG 32
#define POW( x ) ( 1 << ( x ) )
#define Z( x ) ( ( x ) >> 2 )
#define Y( x ) ( ( ( x ) >> 1 ) & 1 )
#define X( x ) ( (x)&1 )
precision highp image3D;
layout( local_size_x = 32, local_size_y = 32 ) in;
uniform highp sampler2D octree;
uniform highp sampler2D terminal;
layout( rgba32f ) uniform writeonly image3D route;
uniform float nNode;
uniform int rootpow;
uniform int start[ 12 ];
uniform int xSize[ 12 ];
uniform int ySize[ 12 ];
uniform vec3 bSpan;
uniform vec3 xAddition;
uniform vec3 yAddition;
uniform vec3 camera;
uniform vec3 direction;
vec3 dir;
float value0;
int oSample( ivec3 offset, int rank, int son )
{
	return int( round( texture( octree, vec2( ( float( son ) + 0.5 ) / 8.0, float( start[ rank ] + offset[ 0 ] + xSize[ rank ] * ( offset[ 1 ] + ySize[ rank ] * offset[ 2 ] ) ) + 0.5 / nNode ) )[ 0 ] ) );
}
vec2 passbox( ivec3 start, int k )
{
	float lowmax = value0;
	float highmin = float( 1 << 14 );
	float tf1;
	float tf2;
	float tf3;
	int p = 0;
	for ( int ii = 0; ii < 3; ii++ ) {
		if ( dir[ ii ] != 0.0 ) {
			tf1 = ( float( start[ ii ] ) * bSpan[ ii ] - camera[ ii ] ) / dir[ ii ];
			tf2 = ( ( float( start[ ii ] + POW( k ) ) ) * bSpan[ ii ] - camera[ ii ] ) / dir[ ii ];
			if ( tf1 > tf2 ) {
				tf3 = tf1;
				tf1 = tf2;
				tf2 = tf3;
			}
			if ( tf1 > lowmax ) lowmax = tf1;
			if ( tf2 < highmin ) highmin = tf2;
		} else {
			if ( !( ( camera[ ii ] >= float( start[ ii ] ) * bSpan[ ii ] ) && ( camera[ ii ] < float( start[ ii ] + POW( k ) ) * bSpan[ ii ] ) ) ) return vec2( 1, 0 );
		}
	}
	return vec2( lowmax, highmin );
}
ivec4 pass8( ivec3 start, int k )
{
	start = start * POW( k );
	vec2 pass0;
	pass0 = passbox( start, k );
	ivec4 res = ivec4( -1, -1, -1, -1 );
	if ( ( pass0[ 1 ] - pass0[ 0 ] ) <= 0.0 ) return res;
	vec3 entry = camera + dir * pass0[ 0 ];
	vec3 passplate;
	ivec3 plates;
	int p = 0;
	float kp;
	res[ 0 ] = int( float( ( start[ 2 ] + POW( k - 1 ) ) ) * bSpan[ 2 ] <= entry[ 2 ] ) * 4 + int( float( ( start[ 1 ] + POW( k - 1 ) ) ) * bSpan[ 1 ] <= entry[ 1 ] ) * 2 + int( float( ( start[ 0 ] + POW( k - 1 ) ) ) * bSpan[ 0 ] <= entry[ 0 ] );
	for ( int ii = 0; ii < 3; ii++ ) {
		if ( dir[ ii ] == 0.0 ) continue;
		kp = ( float( start[ ii ] + POW( k - 1 ) ) * bSpan[ ii ] - camera[ ii ] ) / dir[ ii ];
		if ( kp >= pass0[ 1 ] || kp < pass0[ 0 ] ) continue;
		passplate[ p ] = kp;
		plates[ p ] = ii;
		p++;
	}
	float tf;
	int ti;
	for ( int ii = 0; ii < p; ii++ ) {
		for ( int ij = ii + 1; ij < p; ij++ ) {
			if ( passplate[ ii ] > passplate[ ij ] ) {
				tf = passplate[ ii ];
				passplate[ ii ] = passplate[ ij ];
				passplate[ ij ] = tf;
				ti = plates[ ii ];
				plates[ ii ] = plates[ ij ];
				plates[ ij ] = ti;
			}
		}
	}
	for ( int ii = 0; ii < p; ii++ ) { res[ ii + 1 ] = res[ ii ] ^ POW( plates[ ii ] ); }
	return res;
}
void main( void )
{
	value0 = texture( terminal, vec2( ( float( gl_GlobalInvocationID.x ) + 0.5 ) / 512.0, ( float( gl_GlobalInvocationID.y ) + 0.5 ) / 512.0 ) )[ 0 ];
	dir = direction + ( float( gl_GlobalInvocationID.x ) - 255.5 ) * xAddition + ( float( gl_GlobalInvocationID.y ) - 255.5 ) * yAddition;
	dir = dir / length( dir );
	ivec3 sv[ 36 ];
	int sp[ 36 ];
	int sl = 1;
	int ii;
	ivec4 res[ TAILLONG ];
	int resp = 0;
	for ( int iii = 0; iii < 32; iii++ ) res[ iii ] = ivec4( -1, -1, -1, -1 );
	sv[ 0 ] = ivec3( 0, 0, 0 );
	sp[ 0 ] = rootpow;
	while ( sl > 0 ) {
		sl--;
		ivec3 tv = sv[ sl ];
		int tp = sp[ sl ];
		ivec4 sons = pass8( tv, tp );
		for ( int ii = 3; ii >= 0; ii-- ) {
			if ( sons[ ii ] != -1 ) {
				if ( oSample( tv, tp, sons[ ii ] ) != 0 ) {
					ivec3 son = 2 * tv + ivec3( X( sons[ ii ] ), Y( sons[ ii ] ), Z( sons[ ii ] ) );
					if ( tp == 1 ) {
						if ( resp < 32 ) res[ resp++ ] = ivec4( son, tp - 1 );
					} else {
						sv[ sl ] = son;
						sp[ sl ] = tp - 1;
						sl++;
					}
				}
			}
		}
	}
	for ( int iii = 0; iii < 32; iii++ ) imageStore( route, ivec3( gl_GlobalInvocationID.xy, iii ), vec4( res[ iii ] ) );
}

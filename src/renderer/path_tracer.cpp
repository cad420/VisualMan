
#include "path_tracer.h"
#include "../graphicfoundation/interaction.h"
#include "../graphicfoundation/light.h"
#include "../graphicfoundation/appearance.h"
#include "../graphicfoundation//scene.h"

#include <random>
#include <ctime>
#include "numeric.h"

namespace ysl
{
#define DIRECT_ILLUMINATION
#define GI
std::default_random_engine e( time( 0 ) );
std::uniform_real_distribution<Float> u( Float( 0 ), Float( 1 ) );

RGBASpectrum trace( const Scene &scene,
					const Ray &ray,
					int depth,
					bool di, bool gi,
					int intensity )
{
	RGBASpectrum directIllumination( 0.0 );
	Float t;
	Interaction isect;
	RGBASpectrum indirectIllumination( 0.0 );
	if ( scene.intersect( ray, &t, &isect ) == false ) {
		return RGBASpectrum( 0.0 );
	}
	RGBASpectrum emission = isect.bsdf()->emmision();
	if ( depth == 0 ) {
		//return material emission
		return emission;
	}
	const std::shared_ptr<Material> &m = isect.object()->getMaterial();
#ifdef DIRECT_ILLUMINATION
	if ( di == true ) {
		const auto &lights = scene.lights();
		int lightCount = lights.size();
		int lightSamples = 1;
		//direct light illumination
		for ( const auto &light : lights ) {
			Vector3f wi;

			Float pdf;

			VisibilityTester vis;

			for ( int i = 0; i < lightSamples; i++ ) {
				Point2f sample( u( e ), u( e ) );
				light->sampleLi( isect, &wi, &pdf, sample, &vis );

				if ( vis.occlude( scene ) == false ) {
					//qDebug() << "sample on light";
					RGBASpectrum li = light->L( isect, wi );
					// qDebug() << "Light0:"<<li;
					const Float cosTheta = Vector3f::Dot( vis.to().normal().Normalized(), -wi.Normalized() );
					li = li * cosTheta / vis.distanceSquare() * intensity;
					//qDebug() <<" "<<"Light1:"<< li;
					if ( m != nullptr ) {
						auto ks = m->m_ks;
						auto ka = m->m_ka;
						const auto ns = m->m_ns;
						auto kd = m->m_kd;
						auto v = -ray.Direction().Normalized();
						auto n = isect.normal().Normalized();
						auto l = wi.Normalized();
						auto h = ( v + l ).Normalized();
						switch ( m->m_type ) {
						case MaterialType::Mirror:
							break;
						case MaterialType::Metal:
							directIllumination += ka * li + kd * ( std::max( Vector3f::Dot( n, l ), 0.0f ) ) * li + ks * std::pow( ( std::max( Vector3f::Dot( n, h ), 0.f ) ), ns ) * li;
							break;
						case MaterialType::Glass:
							break;
						default:
							assert( false );
							break;
						}
					}
				}
			}
		}

		directIllumination = directIllumination / static_cast<Float>( lightSamples * lightCount );
	}

#endif
	//indirect light illumination
#ifdef GI
	if ( gi == true ) {
		RGBASpectrum bsdf{ 0.0f };
		Vector3f wi;
		Float pdf;
		Point2f sample( u( e ), u( e ) );
		//qDebug() << sample;
		//qDebug() << (m == nullptr ? false : true);
		switch ( m->m_type ) {
		case MaterialType::Mirror:
			bsdf = isect.bsdf()->sampleF( -ray.Direction(), &wi, &pdf, sample, BSDF_SPECULAR );
			break;
		case MaterialType::Metal:
			bsdf = isect.bsdf()->sampleF( -ray.Direction(), &wi, &pdf, sample, BSDF_DIFFUSE );
			break;
		case MaterialType::Glass:
			bsdf = isect.bsdf()->sampleF( -ray.Direction(), &wi, &pdf, sample, BSDF_REFRACTION );
			break;
		default:
			assert( false );
			break;
		}
		Ray newRay = isect.spawnRay( wi );
		indirectIllumination = bsdf * trace( scene, newRay, depth - 1, di, gi, intensity );
		indirectIllumination = Clamp( indirectIllumination, RGBASpectrum( 0.0f ), RGBASpectrum( 255.f ) );
	}

#endif
	return ( emission + directIllumination + indirectIllumination );
}

}  // namespace ysl

#pragma once

#include <glm/glm.hpp>

namespace Renderer
{
	enum class FrustumPlanes
	{
		FRUSTUM_PLANE_NEAR = 0,
		FRUSTUM_PLANE_FAR = 1,
		FRUSTUM_PLANE_LEFT = 2,
		FRUSTUM_PLANE_RIGHT = 3,
		FRUSTUM_PLANE_TOP = 4,
		FRUSTUM_PLANE_BOTTOM = 5
	};

	struct Plane
	{
		glm::vec3 n;
		float	  d;
	};

	struct Frustum
	{
		Plane planes[6];
	};

	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
	};

	inline void FrustumFromMatrix(Frustum& frustum, const glm::mat4& viewProjMat)
	{
		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_RIGHT].n = glm::vec3(viewProjMat[0][3] - viewProjMat[0][0],
			viewProjMat[1][3] - viewProjMat[1][0],
			viewProjMat[2][3] - viewProjMat[2][0]);
		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_RIGHT].d = viewProjMat[3][3] - viewProjMat[3][0];

		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_LEFT].n = glm::vec3(viewProjMat[0][3] + viewProjMat[0][0],
			viewProjMat[1][3] + viewProjMat[1][0],
			viewProjMat[2][3] + viewProjMat[2][0]);
		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_LEFT].d = viewProjMat[3][3] + viewProjMat[3][0];

		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_BOTTOM].n = glm::vec3(viewProjMat[0][3] + viewProjMat[0][1],
			viewProjMat[1][3] + viewProjMat[1][1],
			viewProjMat[2][3] + viewProjMat[2][1]);
		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_BOTTOM].d = viewProjMat[3][3] + viewProjMat[3][1];

		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_TOP].n = glm::vec3(viewProjMat[0][3] - viewProjMat[0][1],
			viewProjMat[1][3] - viewProjMat[1][1],
			viewProjMat[2][3] - viewProjMat[2][1]);
		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_TOP].d = viewProjMat[3][3] - viewProjMat[3][1];

		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_FAR].n = glm::vec3(viewProjMat[0][3] - viewProjMat[0][2],
			viewProjMat[1][3] - viewProjMat[1][2],
			viewProjMat[2][3] - viewProjMat[2][2]);
		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_FAR].d = viewProjMat[3][3] - viewProjMat[3][2];

		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_NEAR].n = glm::vec3(viewProjMat[0][3] + viewProjMat[0][2],
			viewProjMat[1][3] + viewProjMat[1][2],
			viewProjMat[2][3] + viewProjMat[2][2]);
		frustum.planes[(int)FrustumPlanes::FRUSTUM_PLANE_NEAR].d = viewProjMat[3][3] + viewProjMat[3][2];

		// normalize them
		for (int i = 0; i < 6; ++i)
		{
			float invl = sqrt(frustum.planes[i].n.x * frustum.planes[i].n.x + frustum.planes[i].n.y * frustum.planes[i].n.y + 
				frustum.planes[i].n.z * frustum.planes[i].n.z);
			frustum.planes[i].n /= invl;
			frustum.planes[i].d /= invl;
		}
	}

	inline void ExtractFrustumCorners(const glm::mat4& invViewProjMat, glm::vec3* corners)
	{
		const glm::vec4 kFrustumCorners[] = {
			glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
			glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
			glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
			glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
			glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
			glm::vec4(1.0f, -1.0f, -1.0f, 1.0f)
		};

		for (int i = 0; i < 8; ++i)
		{
			glm::vec4 v = invViewProjMat * kFrustumCorners[i];
			v = v / v.w;
			corners[i] = glm::vec3(v.x, v.y, v.z);
		}
	}

	inline float Classify(const Plane& plane, const AABB& aabb)
	{
		glm::vec3 center = (aabb.max + aabb.min) / 2.0f;
		glm::vec3 extents = aabb.max - aabb.min;

		float r = std::abs(extents.x * plane.n.x) +
			std::abs(extents.y * plane.n.y) +
			std::abs(extents.z * plane.n.z);

		float d = glm::dot(plane.n, center) + plane.d;

		if (std::abs(d) < r)
		{
			return 0.0f;
		}
		else if (d < 0.0f)
		{
			return d + r;
		}
		else
		{
			return d - r;
		}
	}

	inline bool Intersects(const Frustum& frustum, const AABB& aabb)
	{
		for (int i = 0; i < 6; ++i)
		{
			if (Classify(frustum.planes[i], aabb) < 0.0f)
			{
				return false;
			}
		}

		return true;
	}
}

#pragma once

#include "Plane.hpp"

#include <vector>

namespace CommonUtilities
{
	template <class T>
	class PlaneVolume
	{
	public:
		// Default constructor: empty PlaneVolume.
		PlaneVolume()
		{
			myPlanes = { };
		}

		// Constructor taking a list of Planes that makes up the PlaneVolume.
		PlaneVolume(const std::vector<Plane<T>>& aPlaneList)
		{
			myPlanes = aPlaneList;
		}

		// Add a Plane to the PlaneVolume.
		void AddPlane(const Plane<T>& aPlane)
		{
			myPlanes.push_back(aPlane);
		}

		// Returns whether a point is inside the PlaneVolume: it is inside when the point ison the
		// plane or on the side the normal is pointing away from for all the planes in the PlaneVolume.
		bool IsInside(const CommonUtilities::Vector3<T>& aPosition) const
		{
			for (int i = 0; i < myPlanes.size(); i++)
			{
				if (!myPlanes[i].IsInside(aPosition))
				{
					return false;
				}
			}

			return true;
		}

	private:
		std::vector<Plane<T>> myPlanes;
	};
}

#pragma once

#include "Line.hpp"

#include <vector>


namespace CommonUtilities
{
	template <class T>
	class LineVolume
	{
	public:
		// Default constructor: empty LineVolume.
		LineVolume()
		{
			myLines = {};
		}

		// Constructor taking a list of Line that makes up the LineVolume.
		LineVolume(const std::vector<Line<T>>& aLineList)
		{
			for (int i = 0; i < aLineList.size(); i++)
			{
				myLines.push_back(aLineList[i]);
			}
		}

		// Add a Line to the LineVolume.
		void AddLine(const Line<T>& aLine)
		{
			myLines.push_back(aLine);
		}

		// Returns whether a point is inside the LineVolume: it is inside when the point is
		// on the line or on the side the normal is pointing away from for all the lines in
		// the LineVolume.
		bool IsInside(const CommonUtilities::Vector2<T>& aPosition) const
		{
			for (int i = 0; i < myLines.size(); i++)
			{
				if (!myLines[i].IsInside(aPosition))
				{
					return false;
				}
			}

			return true;
		}


	private:
		std::vector<Line<T>> myLines;
	};

}
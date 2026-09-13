#ifndef UTILITIES_H
#define UTILITIES_H

// Indices (locations) of queue families (if they exist at all)
struct QueueFamilyIndices 
{
	int graphicsFamily = -1;	// Location of the graphics queue family

	// Check if queue families are valid
	bool isValid() const
	{
		return graphicsFamily >= 0;
	}
};

#endif // !UTILITIES_H

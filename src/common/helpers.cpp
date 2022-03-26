#include "iostream"
#include <vector>
#include <algorithm>

// Function used to sort the sprites
// Sort the sprites based on distance
void sortSprites(int *order, int *dist, int amount)
{
	std::vector<std::pair<int, int> > sprites(amount);
	for (int i = 0; i < amount; i++)
	{
		sprites[i].first = dist[i];
		sprites[i].second = order[i];
	}
	sort(sprites.begin(), sprites.end());
	// restore in reverse order to go from farthest to nearest
	for (int i = 0; i < amount; i++)
	{
		dist[i] = sprites[amount - i - 1].first;
		order[i] = sprites[amount - i - 1].second;
	}
}

// Clip float value in range
float clipf(float n, float lower, float upper)
{
	return std::max(lower, std::min(n, upper));
}

// Clip int value in range
int clip(int n, int lower, int upper)
{
	return std::max(lower, std::min(n, upper));
}

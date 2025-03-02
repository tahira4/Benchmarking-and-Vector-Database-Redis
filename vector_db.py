# Install required libraries (run these in terminal if not installed)
# pip install redis numpy  # Install Redis and NumPy libraries
# pip show redis  # Check if the Redis library is installed

import redis  # Import Redis library to interact with the Redis database
import numpy as np  # Import NumPy for handling numerical computations

# Connect to the Redis database
r = redis.Redis(host='localhost', port=6379, decode_responses=False)

# Function to calculate cosine similarity between two vectors
# Cosine similarity measures how similar two vectors are based on the angle between them
def cosine_similarity(v1, v2):
    return np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2))

# Clear all previous data from Redis to ensure a fresh start
r.flushall()
"""
Define vector attributes for different charms
Each charm is represented as a 4D vector with attributes:
- Magical power
- Luck level
- Rarity
- Glow intensity
"""
charms = {
    "FireCharm": np.array([0.9, 0.3, 0.7, 0.5], dtype=np.float32),  # High power, medium rarity
    "LuckCharm": np.array([0.2, 0.9, 0.6, 0.8], dtype=np.float32),  # High luck, strong glow
    "ShadowCharm": np.array([0.5, 0.2, 0.9, 0.3], dtype=np.float32)  # High rarity, low glow
}

# Store each charm in Redis with its vector converted to bytes
for charm_name, vector in charms.items():
    r.set(f"charm:{charm_name}", vector.tobytes())  # Convert NumPy array to bytes for storage
    print(f"Added charm: {charm_name} with vector: {vector}")  # Print confirmation

# Define a query vector (representing an unknown charm for similarity comparison)
# This vector represents a new charm with unknown category but similar attributes
query_vector = np.array([0.4, 0.7, 0.5, 0.6], dtype=np.float32)
print(f"\nQuery Vector: {query_vector}")  # Display query vector

# Find the most similar charms by calculating cosine similarity
results = []
for charm_name, vector in charms.items():
    similarity = cosine_similarity(query_vector, vector)  # Compute similarity score
    results.append((charm_name, similarity))  # Store results

# Sort results in descending order based on similarity score
results.sort(key=lambda x: x[1], reverse=True)

# Display the similarity results
print("\nSimilarity Search Results:")
for charm_name, similarity in results:
    print(f"Charm: {charm_name}, Similarity: {similarity:.4f}")  # Print sorted results

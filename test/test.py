# Make sure that sgre.so is reachable using PYTHONPATH env variable
import sgre

# Read configuration as a string
with open('config.json', 'rb') as f:
    config = f.read()

# Read GeoJSON geometry as a string
with open('navigation.geojson', 'rb') as f:
    geojson = f.read()

# Define name of the profile (use empty string if profiles are not used)
profile = ''

# Create RouteFinder object. Note that RouteFinder should be reused
# as much as possible, as creating a new instance is an expensive operation.
router = sgre.create_routefinder(geojson, config, profile)

# Define routing origin and destination points and create query
origin = sgre.Point(-5.99632725596723, 37.3932610196257, 0)
destination = sgre.Point(-5.99673452489332, 37.3932966870052, 12)
query = sgre.Query(origin, destination)
query.origin_filter = {'type': 'hallway'}
query.destination_filter = {'type': 'hallway'}

# Find the route
result = router.find(query)

# Print various properties of the result (to demonstrate API)
print('Status: %s' % result.status)
print('Instruction count: %d' % len(result.instructions))
print('First instruction type: %s' % result.instructions[0].type)
print('Total time: %g' % result.totaltime)
print('Full result: %s' % result)
print('Initial point: %s' % result.geometry[result.instructions[0].geometryindex])
print('Final point: %s' % result.geometry[result.instructions[-1].geometryindex])
print(router)

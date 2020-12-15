import sys  # command line arguments
			# sys.argv is a list with arguments ex:['schedsim.py', 'arg1', 'arg2']
import copy


# sys.argv[1] topology source file
# sys.argv[2] how many rounds to run
# sys.argv[3] trace packet start node
# sys.argv[4] trace packet finish node
debug = False
filename = sys.argv[1]
MAX_ROUNDS = int(sys.argv[2])
edge_list = []

f = open(filename, "r")
for row in f:
	edge_list.append([int(x) for x in row.split()])

"""##########           CONSTRUCT THE TOPOLOGY              ##########"""

nodes = []

for edge in edge_list:
	if edge[0] not in nodes:
		nodes.append(edge[0])
	if edge[1] not in nodes:
		nodes.append(edge[1])

nodes.sort()
print("Nodes: ", nodes)
print("Edges: ", edge_list)

"""##########           GENERATE STARTING DV TABLES         ##########"""

initial_dv_tables = []

# when populated, dv_table should look like:
#   [node0,
#       [[node1, cost, node1], [node2, cost, node2]],
#           [neighbor1, neighbor2,...]]
#   [node1,
#       [[node0, cost, node0]]...

# check each node pair in the initial topology to see which neighbors are initially known, then add those to each nodes dv_table entry
for node in nodes:
	dv_entry = []
	neighbor_list = []
	# entries in the table will be [destination, cost, next_hop]
	for edge_pair in edge_list:
		if edge_pair[0] == node:
			dv_entry.append([edge_pair[1], edge_pair[2], edge_pair[1]])
			neighbor_list.append(edge_pair[1])
		elif edge_pair[1] == node:
			dv_entry.append([edge_pair[0], edge_pair[2], edge_pair[0]])
			neighbor_list.append(edge_pair[0])

	initial_dv_tables.append([node, dv_entry, neighbor_list])
dv_tables = copy.deepcopy(initial_dv_tables)
if debug:
	print("Initial DV Table: ")
	for entry in initial_dv_tables:
		print(entry)

"""##########           BEGIN DISSEMINATION SIMULATION          ##########"""

ROUNDS = 0
full_topology = -1
last_update = -1
last_node = 0
packets_sent = -1
total_packets_sent = -1

# convergence occurs when each nodes has n-1 entries in their table, where n is # of nodes

while ROUNDS < MAX_ROUNDS:
	ROUNDS += 1
	# "dv_packets" are a list already, but we need to change all the values for easy comparison.
	# In this step, a node "sends" its dv packet to its neighbors.
	# The neighbor then compares it to what it knows. If there is new info, the node copies that entry, but changes
	# "cost" to add in cost to sender, and changes "next_hop" to factor sender.
	# If the connection is not new it must still be compared to the cost of the previous entry
	if debug:
		print("\nROUND ", ROUNDS, " DV PACKETS -")
	update_table = []
	# update table will store sent dv packets until end of round so that nodes are not being updated before
	# their packets are constructed and sent
	# structure will look like:
	#   [[node0, [[update1], [update2]]]
	for node in nodes:
		update_table.append([node])

	for node in dv_tables:
		# packet will be [sender, cost, [[node_entry], [node_entry]]
		for entry in node[2]:  # prep packet for NEIGHBORS ONLY
			cost = -1
			packets_sent += 1
			for cost_lookup in dv_tables:
				if cost_lookup[0] == node[0]:
					for edge in node[1]:
						if edge[0] == entry:
							cost = edge[1]

			"""" must calc cost"""
			receiver = entry
			sender = node[0]    # the "sender" (this node)

			dv_packet = []
			for other_entry in node[1]:  # add all entries where "next_hop" is not the destination node

				if other_entry[2] != receiver and other_entry[0] != receiver:
					dv_packet.append([other_entry[0], other_entry[1] + cost, sender])
			if debug:
				print("\nSender: ", sender, "\nReceiver: ", receiver, "\nPacket: ", dv_packet)

			for tab in update_table:
				if tab[0] == receiver:
					if len(tab) < 2:
						tab.append(dv_packet)
					else:
						for destination in dv_packet:
							tab[1].append(destination)
	if debug:
		print("\nUpdate Table:")
		for tab in update_table:
			print(tab)
	# At this point, each node's "inbox" needs to be read analyzed
	# [[0, [[4, 1439, 1], [3, 931, 1]], [[4, 960, 2], [3, 1013, 2]]], [1, [[2, 1198, 0]]..
	for tab in update_table:
		for packet in tab[1]:
			# for each packet in each update_table tab, compare to dv_table node table entries
			for node in dv_tables:
				if node[0] == tab[0]:
					new_destination = True
					for dv_table_entry in node[1]:
						if dv_table_entry[0] == packet[0]:
							if dv_table_entry[1] > packet[1]:
								dv_table_entry[1] = packet[1]
								dv_table_entry[2] = packet[2]
								if debug:
									print("Better route found!")
								last_update = ROUNDS
								last_node = tab[0]
								total_packets_sent += packets_sent
								packets_sent = 0
							else:
								if debug:
									print("unhelpful route found..")
							new_destination = False
					if new_destination:
						node[1].append(packet)
						if debug:
							print("New Destination!")
							last_update = ROUNDS
							last_node = tab[0]
							total_packets_sent += packets_sent
							packets_sent = 0

	print("Round ", ROUNDS, "DV Table: ")
	for tab in dv_tables:
		print(tab[0], tab[1])

	num_nodes_with_full_top = 0
	for node in dv_tables:
		if len(node[1]) == len(nodes) - 1:
			num_nodes_with_full_top += 1

	if num_nodes_with_full_top == len(nodes) and full_topology < 0:
		full_topology = ROUNDS
		print ("SOFT CONVERGENCE ACHIEVED IN ", ROUNDS, " ROUND(S)")

print("CONVERGENCE ACHIEVED IN ", last_update, " ROUND(S)")
print("Last node to update: ", last_node)
print("Total packets sent: ", total_packets_sent)

if debug:
	print("initial")
	for x in initial_dv_tables:
		print(x)

"""##########           Packet Routing Simulation          ##########"""
if len(sys.argv) >= 5:
	node1 = int(sys.argv[3])
	destination = int(sys.argv[4])

	route = [node1]

	destination_reached = False
	current_node = node1

	while not destination_reached:
		for node in dv_tables:
			if node[0] == current_node:
				for r_node in node[1]:
					if r_node[0] == destination:
						route.append(r_node[2])
						current_node = r_node[2]

		if current_node == destination:
			destination_reached = True

	print("\nNode ", node1, " to Node ", destination, " Route: ", route)


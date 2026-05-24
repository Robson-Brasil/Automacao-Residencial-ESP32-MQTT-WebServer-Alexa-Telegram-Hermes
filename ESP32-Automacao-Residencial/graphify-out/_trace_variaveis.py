import json, sys
import networkx as nx
from networkx.readwrite import json_graph
from pathlib import Path

sys.stdout.reconfigure(encoding='utf-8')

data = json.loads(Path('graphify-out/graph.json').read_text(encoding='utf-8'))
G = json_graph.node_link_graph(data, edges='links')

# VariaveisGlobais.h (Global State Header)
nid = 'VariaveisGlobais_h'
label = G.nodes[nid].get('label', nid)
print(f"NODE: {label}")
print(f"  source: {G.nodes[nid].get('source_file','')}")
print(f"  type: {G.nodes[nid].get('file_type','')}")
print(f"  degree: {G.degree(nid)}")
print()

# Get community membership
analysis = json.loads(Path('graphify-out/.graphify_analysis.json').read_text(encoding='utf-8'))
node_to_community = {}
for cid_str, members in analysis['communities'].items():
    for m in members:
        node_to_community[m] = int(cid_str)

comm_labels_raw = Path('graphify-out/.graphify_labels.json').read_text(encoding='utf-8')
import json as j2
comm_labels = j2.loads(comm_labels_raw)

my_comm = node_to_community.get(nid, -1)
print(f"Community: {comm_labels.get(str(my_comm), f'Community {my_comm}')} (id={my_comm})")
print()

print("CONNECTIONS:")
for neighbor in G.neighbors(nid):
    raw = G[nid][neighbor]
    if isinstance(G, nx.MultiGraph):
        edge = next(iter(raw.values()), {})
    else:
        edge = raw
    nlabel = G.nodes[neighbor].get('label', neighbor)
    rel = edge.get('relation', '')
    conf = edge.get('confidence', '')
    src = G.nodes[neighbor].get('source_file', '')
    ncomm = node_to_community.get(neighbor, -1)
    ncomm_name = comm_labels.get(str(ncomm), f'Community {ncomm}')
    cross = " [CROSS-COMMUNITY]" if ncomm != my_comm else ""
    print(f"  --{rel}--> {nlabel} ({conf}){cross}")
    print(f"             file={src}, community={ncomm_name}")

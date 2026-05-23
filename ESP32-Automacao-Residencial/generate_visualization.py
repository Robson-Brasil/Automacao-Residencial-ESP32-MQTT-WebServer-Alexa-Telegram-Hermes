import json
import networkx as nx
from networkx.readwrite import json_graph
from pyvis.network import Network
from pathlib import Path

# Carregar grafo
data = json.loads(Path('graphify-out/graph.json').read_text())
G = json_graph.node_link_graph(data, edges='links')

# Comunidades (hardcoded — copiado da análise inicial)
communities = {
    node: 0 for node in G.nodes()  # Default (ajustar manualmente se necessário)
}
# Ajustar comunidades com base nos nós mais importantes
for node in G.nodes():
    if 'ESP32' in G.nodes[node].get('label', '') or 'MQTT' in G.nodes[node].get('label', ''):
        communities[node] = 0
    elif 'Telegram' in G.nodes[node].get('label', ''):
        communities[node] = 1
    elif 'Alexa' in G.nodes[node].get('label', ''):
        communities[node] = 2
    elif '.py' in G.nodes[node].get('source_file', ''):
        communities[node] = 3
    else:
        communities[node] = 4

# Rótulos
label_map = {
    0: "Automação MQTT (ESP32)",
    1: "Integração Telegram",
    2: "Configuração Alexa",
    3: "Scripts Python (Skills)",
    4: "Documentação"
}

# Gerar HTML
net = Network(notebook=False, directed=True, height="750px", width="100%")
for node_id, node_data in G.nodes(data=True):
    label = node_data.get('label', node_id)
    net.add_node(node_id, label=label, title=node_id, group=label_map[communities[node_id]])
for u, v, edge_data in G.edges(data=True):
    net.add_edge(u, v, title=edge_data.get('relation', ''))

net.write_html('graphify-out/graph.html')
net.write_svg('graphify-out/graph.svg')

print('✅ Visualizações geradas em graphify-out/')

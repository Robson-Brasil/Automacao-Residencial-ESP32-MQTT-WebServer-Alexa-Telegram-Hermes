#!/usr/bin/env python3
import json
import networkx as nx
from networkx.readwrite import json_graph
from pathlib import Path

# Carregar grafo existente
data = json.loads(Path('graphify-out/graph.json').read_text())
G = json_graph.node_link_graph(data, edges='links')

# Carregar análise (communities)
analysis = json.loads(Path('.graphify_analysis.json').read_text())
communities = {int(k): v for k, v in analysis['communities'].items()}

# Rótulos das comunidades (baseado nos "God Nodes" da análise)
label_map = {
    0: "Automação MQTT (ESP32)",
    1: "Integração Telegram",
    2: "Configuração Alexa",
    3: "Scripts Python (Skills)",
    4: "Documentação (Node_modules/SKILL.md)"
}

# Atualizar relatório
if Path('graphify-out/GRAPH_REPORT.md').exists():
    report = Path('graphify-out/GRAPH_REPORT.md').read_text()
    for cid, label in label_map.items():
        report = report.replace(f'Comunidade {cid}', label)
    Path('graphify-out/GRAPH_REPORT.md').write_text(report)

# Gerar HTML/SVG
def to_html(G, communities, output_path, community_labels=None):
    from pyvis.network import Network
    net = Network(notebook=False, directed=True)
    for node_id, node_data in G.nodes(data=True):
        label = node_data.get('label', node_id)
        net.add_node(node_id, label=label, title=node_id,
                    group=community_labels.get(communities.get(node_id), 'Unknown'))
    for u, v, edge_data in G.edges(data=True):
        net.add_edge(u, v, title=edge_data.get('relation', ''))
    net.write_html(output_path)

def to_svg(G, communities, output_path, community_labels=None):
    from pyvis.network import Network
    net = Network(notebook=False, directed=True, height="750px", width="100%")
    for node_id, node_data in G.nodes(data=True):
        label = node_data.get('label', node_id)
        net.add_node(node_id, label=label, title=node_id,
                    group=community_labels.get(communities.get(node_id), 'Unknown'))
    for u, v, edge_data in G.edges(data=True):
        net.add_edge(u, v, title=edge_data.get('relation', ''))
    net.write_svg(output_path)

to_html(G, communities, 'graphify-out/graph.html', community_labels=label_map)
to_svg(G, communities, 'graphify-out/graph.svg', community_labels=label_map)

print('✅ Tudo pronto, Senhor!')
print('Arquivos gerados em graphify-out/')
print('- graph.html (visualização interativa)')
print('- graph.svg (imagem vetorial)')
print('- GRAPH_REPORT.md (relatório atualizado)')

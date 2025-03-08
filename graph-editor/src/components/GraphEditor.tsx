import React, { useCallback, useState } from "react";
import ReactFlow, {
  addEdge,
  useNodesState,
  useEdgesState,
  Controls,
  Node,
  Edge,
} from "reactflow";
import "reactflow/dist/style.css";
import nodeConfigs from "../nodeConfigs";
import { GraphNode } from "../types";
import CustomNode from "./CustomNode"

const nodeTypes = {custom: CustomNode};

const GraphEditor: React.FC = () => {
  const [nodes, setNodes, onNodesChange] = useNodesState<GraphNode[]>([]);
  const [edges, setEdges, onEdgesChange] = useEdgesState<Edge[]>([]);
  const [selectedNode, setSelectedNode] = useState<GraphNode | null>(null);

  const onConnect = useCallback(
    (params) => setEdges((eds) => addEdge(params, eds)),
    []
  );

  const onDrop = useCallback(
    (event: React.DragEvent<HTMLDivElement>) => {
      event.preventDefault();
      const nodeData = JSON.parse(event.dataTransfer.getData("application/reactflow"));
      if (!nodeData) return;

      const position = { x: event.clientX - 250, y: event.clientY - 50 };
      const config = nodeConfigs[nodeData.id] || { name: nodeData.label, config: {} };

      const newNode: GraphNode = {
        id: `${nodeData.id}-${nodes.length}`,
        type: "custom",
        position,
        data: { label: config.name, config: config.config },
      };

      setNodes((nds) => [...nds, newNode]);
    },
    [setNodes, nodes]
  );

  const saveGraphToJson = () => {
    const graphData = { nodes, edges };
    const jsonString = JSON.stringify(graphData, null, 2);
    const blob = new Blob([jsonString], { type: "application/json" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = "graph.json";
    a.click();
    URL.revokeObjectURL(url);
  };

  const loadGraphFromJson = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (e) => {
      const graphData = JSON.parse(e.target?.result as string);
      setNodes(graphData.nodes || []);
      setEdges(graphData.edges || []);
    };
    reader.readAsText(file);
  };

  return (
    <div style={{ display: "flex", height: "100vh", width: "100%"}}>
      <div style={{ flexGrow: 1, height: "100vh" }} onDrop={onDrop} onDragOver={(event) => event.preventDefault()}>
        <ReactFlow
          nodes={nodes}
          edges={edges}
          nodeTypes={nodeTypes}
          onNodesChange={onNodesChange}
          onEdgesChange={onEdgesChange}
          onConnect={onConnect}
        >
          <Controls />
        </ReactFlow>

        <div style={{ position: "absolute", top: 10, left: 10, zIndex: 10 }}>
          <button onClick={saveGraphToJson} style={{ marginRight: 10 }}>Save as JSON</button>
          <input type="file" onChange={loadGraphFromJson} accept=".json" />
        </div>
      </div>
    </div>
  );
};

export default GraphEditor;


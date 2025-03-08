import React from "react";
import nodeConfigs from "../nodeConfigs";

const Sidebar: React.FC = () => {
  const onDragStart = (event: React.DragEvent<HTMLDivElement>, nodeType: string) => {
    event.dataTransfer.setData("application/reactflow", JSON.stringify({ id: nodeType, ...nodeConfigs[nodeType] }));
    event.dataTransfer.effectAllowed = "move";
  };

  return (
    <aside style={{ padding: 10, width: 150, background: "#f4f4f4" }}>
      <h3>Nodes</h3>
      {Object.keys(nodeConfigs).map((key) => (
        <div
          key={key}
          draggable
          onDragStart={(event) => onDragStart(event, key)}
          style={{
            borderRadius: 5,
            padding: 10,
            margin: "5px 0",
            background: "#ddd",
            cursor: "grab",
          }}
        >
          {nodeConfigs[key].name}
        </div>
      ))}
    </aside>
  );
};

export default Sidebar;


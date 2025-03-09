import React from "react";
import nodeConfigs from "../nodeConfigs";
import { theme } from "../theme";

const Sidebar: React.FC = () => {
  const onDragStart = (event: React.DragEvent<HTMLDivElement>, nodeType: string) => {
    event.dataTransfer.setData("application/reactflow", JSON.stringify({ id: nodeType, ...nodeConfigs[nodeType] }));
    event.dataTransfer.effectAllowed = "move";
  };

  return (
    <aside style={{
        padding: 10,
        width: 150,
        background: theme.colors.sidebar_bg,
        color: theme.colors.primary,
        fontFamily: theme.fonts.primary }}>
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
            background: theme.colors.nodeBg,
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


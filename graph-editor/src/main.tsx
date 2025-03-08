import React from "react";
import ReactDOM from "react-dom/client";
import Sidebar from "./components/Sidebar";
import GraphEditor from "./components/GraphEditor";
import { ReactFlowProvider } from "reactflow";

const App: React.FC = () => {
  return (
    <div style={{ display: "flex", height: "100vh" }}>
      <Sidebar />
      <GraphEditor />
    </div>
  );
};

ReactDOM.createRoot(document.getElementById("root")!).render(<App />);

'use client'
import { getTasks } from "@/actions"
import { Task, getColumns } from "./columns"
import { DataTable } from "./data-table"
import { useEffect, useState } from "react"

async function getData(): Promise<Task[]> {
    // Fetch data from your API here.
    const tasks = await getTasks()

    return tasks.data && tasks.data.map(task => {
        return {
            id: task.id,
            task_type: task.task_type,
            task_date: task.task_date,
            task_time: task.task_time,
            trigger_type: task.trigger_type,
            status: task.status,
            createdAt: new Date(task.created_at).toISOString().split("T")[0],
        }
    }) || []

    // return [
    //     {
    //         id: "728ed52f",
    //         task_type: 'Relay',
    //         task_date: new Date("2022-01-01").toISOString().split("T")[0],
    //         task_time: new Date("2022-01-01").toISOString().split("T")[0],
    //         trigger_type: 'ON',
    //         status: "pending",
    //         createdAt: new Date("2022-01-01").toISOString().split("T")[0],
    //     },
    //     // ...
    // ]
}

export default function DemoPage() {
    const [data, setData] = useState<Task[]>([])

    const loadTask = async () => {
        const res = await getData()
        if (res) {
            setData(res)
        }
    }


    useEffect(() => {
        loadTask()
    }, [])

    const columns = getColumns(loadTask)


    return (
        <div className="container mx-auto py-10">
            <DataTable columns={columns} data={data} />
        </div>
    )
}

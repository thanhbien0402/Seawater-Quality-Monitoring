'use client'
import { getTasks } from "@/actions"
import { Task, getColumns } from "./columns"
import { DataTable } from "./data-table"
import { useEffect, useState } from "react"

async function getData(deviceId: string): Promise<Task[]> {
    // Fetch data from your API here.
    const tasks = await getTasks(deviceId)

    return tasks.data && tasks.data.map(task => {
        return {
            id: task.id,
            task_type: task.task_type,
            task_date: task.task_date,
            task_time: task.task_time,
            trigger_type: task.trigger_type,
            status: task.status,
            rpcID: task.rpcID,
            deviceId: task.deviceId,
            createdAt: new Date(task.created_at).toLocaleString("en-GB", {
                timeZone: "Asia/Bangkok",       // UTC+7
                year: "numeric",
                month: "2-digit",
                day: "2-digit",
                hour: "2-digit",
                minute: "2-digit",
                second: "2-digit",
            }),
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

export default function TaskTable({ deviceId }: { deviceId: string }) {
    const [data, setData] = useState<Task[]>([])

    const loadTask = async () => {
        const res = await getData(deviceId)
        if (res) {
            setData(res)
        }
    }


    useEffect(() => {
        loadTask()
    }, [])

    const columns = getColumns(loadTask, deviceId)


    return (
        <div className="container mx-auto py-10">
            <DataTable columns={columns} data={data} />
        </div>
    )
}

'use client'
import { getRanges } from "@/actions"
import { useEffect, useState } from "react"
import { DataTable } from "./data-table"
import { getColumns, Range } from "./columns"

async function getData(deviceId: string): Promise<Range[]> {
    // Fetch data from your API here.
    const ranges = await getRanges('', deviceId)

    return ranges.data && ranges.data.map(range => {
        return {
            id: range.id,
            deviceId: range.deviceId,
            sensor: range.sensor,
            createdAt: new Date(range.created_at).toLocaleString("en-GB", {
                timeZone: "Asia/Bangkok",       // UTC+7
                year: "numeric",
                month: "2-digit",
                day: "2-digit",
                hour: "2-digit",
                minute: "2-digit",
                second: "2-digit",
            }),
            updatedAt: new Date(range.updated_at).toISOString().split("T")[0],
            minValue: range.minValue,
            maxValue: range.maxValue
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

export default function SensorRangeTable({ deviceId }: { deviceId: string }) {
    const [data, setData] = useState<Range[]>([])

    const loadRange = async () => {
        const res = await getData(deviceId)
        if (res) {
            setData(res)
        }
    }


    useEffect(() => {
        loadRange()
    }, [])

    const columns = getColumns(loadRange)


    return (
        <div className="container mx-auto py-10">
            <DataTable columns={columns} data={data} />
        </div>
    )
}

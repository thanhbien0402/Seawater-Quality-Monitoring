'use client'
import { getNotifications } from "@/actions"
import { useEffect, useState } from "react"
import { DataTable } from "./data-table"
import { getColumns, Notification } from "./columns"

async function getData(deviceId: string): Promise<Notification[]> {
    // Fetch data from your API here.
    const notifications = await getNotifications(deviceId)

    return notifications.data && notifications.data.map(noti => {
        return {
            id: noti.id,
            createdAt: new Date(noti.created_at).toLocaleString("en-GB", {
                timeZone: "Asia/Bangkok",       // UTC+7
                year: "numeric",
                month: "2-digit",
                day: "2-digit",
                hour: "2-digit",
                minute: "2-digit",
                second: "2-digit",
            }),
            deviceId: noti.deviceId,
            deviceName: noti.deviceName,
            sensor: noti.sensor,
            condition: noti.condition,
            content: noti.content
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

export default function NotificationDeviceTable({ deviceId }: { deviceId: string }) {
    const [data, setData] = useState<Notification[]>([])

    const loadNotification = async () => {
        const res = await getData(deviceId)
        if (res) {
            setData(res)
        }
    }


    useEffect(() => {
        loadNotification()
    }, [])

    const columns = getColumns(loadNotification)


    return (
        <div className="container mx-auto py-10">
            <DataTable columns={columns} data={data} />
        </div>
    )
}
